// XUnzip.cpp  Version 1.3
//
// Authors:      Mark Adler et al. (see below)
//
// Modified by:  Lucian Wischik
//               lu@wischik.com
//
// Version 1.0   - Turned C files into just a single CPP file
//               - Made them compile cleanly as C++ files
//               - Gave them simpler APIs
//               - Added the ability to zip/unzip directly in memory without 
//                 any intermediate files
// 
// Modified by:  Hans Dietrich
//               hdietrich@gmail.com
//
// Version 1.3:  - Corrected size bug introduced by 1.2
//
// Version 1.2:  - Many bug fixes.  See CodeProject article for list.
//
// Version 1.1:  - Added Unicode support to CreateZip() and ZipAdd()
//               - Changed file names to avoid conflicts with Lucian's files
//
// Modified by:  Julien Templier
//
// Version 1.0   - Merged changes from original zip./unzip files
//
///////////////////////////////////////////////////////////////////////////////
//
// Lucian Wischik's comments:
// --------------------------
// THIS FILE is almost entirely based upon code by Info-ZIP.
// It has been modified by Lucian Wischik.
// The original code may be found at http://www.info-zip.org
// The original copyright text follows.
//
///////////////////////////////////////////////////////////////////////////////
//
// Original authors' comments:
// ---------------------------
// This is version 2002-Feb-16 of the Info-ZIP copyright and license. The 
// definitive version of this document should be available at 
// ftp://ftp.info-zip.org/pub/infozip/license.html indefinitely.
// 
// Copyright (c) 1990-2002 Info-ZIP.  All rights reserved.
//
// For the purposes of this copyright and license, "Info-ZIP" is defined as
// the following set of individuals:
//
//   Mark Adler, John Bush, Karl Davis, Harald Denker, Jean-Michel Dubois,
//   Jean-loup Gailly, Hunter Goatley, Ian Gorman, Chris Herborth, Dirk Haase,
//   Greg Hartwig, Robert Heath, Jonathan Hudson, Paul Kienitz, 
//   David Kirschbaum, Johnny Lee, Onno van der Linden, Igor Mandrichenko, 
//   Steve P. Miller, Sergio Monesi, Keith Owens, George Petrov, Greg Roelofs, 
//   Kai Uwe Rommel, Steve Salisbury, Dave Smith, Christian Spieler, 
//   Antoine Verheijen, Paul von Behren, Rich Wales, Mike White
//
// This software is provided "as is", without warranty of any kind, express
// or implied.  In no event shall Info-ZIP or its contributors be held liable
// for any direct, indirect, incidental, special or consequential damages
// arising out of the use of or inability to use this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//    1. Redistributions of source code must retain the above copyright notice,
//       definition, disclaimer, and this list of conditions.
//
//    2. Redistributions in binary form (compiled executables) must reproduce 
//       the above copyright notice, definition, disclaimer, and this list of 
//       conditions in documentation and/or other materials provided with the 
//       distribution. The sole exception to this condition is redistribution 
//       of a standard UnZipSFX binary as part of a self-extracting archive; 
//       that is permitted without inclusion of this license, as long as the 
//       normal UnZipSFX banner has not been removed from the binary or disabled.
//
//    3. Altered versions--including, but not limited to, ports to new 
//       operating systems, existing ports with new graphical interfaces, and 
//       dynamic, shared, or static library versions--must be plainly marked 
//       as such and must not be misrepresented as being the original source.  
//       Such altered versions also must not be misrepresented as being 
//       Info-ZIP releases--including, but not limited to, labeling of the 
//       altered versions with the names "Info-ZIP" (or any variation thereof, 
//       including, but not limited to, different capitalizations), 
//       "Pocket UnZip", "WiZ" or "MacZip" without the explicit permission of 
//       Info-ZIP.  Such altered versions are further prohibited from 
//       misrepresentative use of the Zip-Bugs or Info-ZIP e-mail addresses or 
//       of the Info-ZIP URL(s).
//
//    4. Info-ZIP retains the right to use the names "Info-ZIP", "Zip", "UnZip",
//       "UnZipSFX", "WiZ", "Pocket UnZip", "Pocket Zip", and "MacZip" for its 
//       own source and binary releases.
//
///////////////////////////////////////////////////////////////////////////////

#include "Config.h"
#if ENABLE_ZIP_FALLBACK

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>
#include <sys/utime.h> // microsoft puts it here
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include "XUnzip.h"

#include "zlib/zlib.h"

#ifdef UNICODE
#define _tsprintf swprintf
#else
#define _tsprintf sprintf
#endif

#pragma warning(disable : 4996)	// disable bogus deprecation warning
#pragma warning(disable : 4702)   // unreachable code

#define zmalloc(len) malloc(len)
#define zfree(p) free(p)

typedef unsigned long lutime_t;       // define it ourselves since we don't include time.h

typedef struct tm_unz_s
{ unsigned int tm_sec;            // seconds after the minute - [0,59]
  unsigned int tm_min;            // minutes after the hour - [0,59]
  unsigned int tm_hour;           // hours since midnight - [0,23]
  unsigned int tm_mday;           // day of the month - [1,31]
  unsigned int tm_mon;            // months since January - [0,11]
  unsigned int tm_year;           // years - [1980..2044]
} tm_unz;

// =============================================================
// some decryption routines
#define CRC32(c, b) (get_crc_table()[((int)(c)^(b))&0xff]^((c)>>8))
void Uupdate_keys(unsigned long *keys, char c)
{ keys[0] = CRC32(keys[0],c);
  keys[1] += keys[0] & 0xFF;
  keys[1] = keys[1]*134775813L +1;
  keys[2] = CRC32(keys[2], keys[1] >> 24);
}

char Udecrypt_byte(unsigned long *keys)
{ unsigned temp = ((unsigned)keys[2] & 0xffff) | 2;
  return (char)(((temp * (temp ^ 1)) >> 8) & 0xff);
}
char zdecode(unsigned long *keys, char c)
{ c^=Udecrypt_byte(keys);
  Uupdate_keys(keys,c);
  return c;
}

#define CRC_DO1(buf) crc = get_crc_table()[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define CRC_DO2(buf)  CRC_DO1(buf); CRC_DO1(buf);
#define CRC_DO4(buf)  CRC_DO2(buf); CRC_DO2(buf);
#define CRC_DO8(buf)  CRC_DO4(buf); CRC_DO4(buf);

uLong ucrc32(uLong crc, const Byte *buf, uInt len)
{ if (buf == Z_NULL) return 0L;
  crc = crc ^ 0xffffffffL;
  while (len >= 8)  {CRC_DO8(buf); len -= 8;}
  if (len) do {CRC_DO1(buf);} while (--len);
  return crc ^ 0xffffffffL;
}

// ----------------------------------------------------------------------
// some windows<->linux portability things
// ----------------------------------------------------------------------
DWORD GetFilePosU(HANDLE hfout)
{ return SetFilePointer(hfout,0,0,FILE_CURRENT);
}

FILETIME timet2filetime(const lutime_t t)
{ LONGLONG i = Int32x32To64(t,10000000) + 116444736000000000LL;
  FILETIME ft;
  ft.dwLowDateTime = (DWORD) i;
  ft.dwHighDateTime = (DWORD)(i >>32);
  return ft;
}

FILETIME dosdatetime2filetime(WORD dosdate,WORD dostime)
{ // date: bits 0-4 are day of month 1-31. Bits 5-8 are month 1..12. Bits 9-15 are year-1980
  // time: bits 0-4 are seconds/2, bits 5-10 are minute 0..59. Bits 11-15 are hour 0..23
  SYSTEMTIME st;
  st.wYear = (WORD)(((dosdate>>9)&0x7f) + 1980);
  st.wMonth = (WORD)((dosdate>>5)&0xf);
  st.wDay = (WORD)(dosdate&0x1f);
  st.wHour = (WORD)((dostime>>11)&0x1f);
  st.wMinute = (WORD)((dostime>>5)&0x3f);
  st.wSecond = (WORD)((dostime&0x1f)*2);
  st.wMilliseconds = 0;
  FILETIME ft; SystemTimeToFileTime(&st,&ft);
  return ft;
}

bool FileExists(const TCHAR *fn)
{ return (GetFileAttributes(fn)!=0xFFFFFFFF);
}
// ----------------------------------------------------------------------



// unz_global_info structure contain global data about the ZIPfile
typedef struct unz_global_info_s
{ unsigned long number_entry;         // total number of entries in the central dir on this disk
  unsigned long size_comment;         // size of the global comment of the zipfile
} unz_global_info;

// unz_file_info contain information about a file in the zipfile
typedef struct unz_file_info_s
{ unsigned long version;              // version made by                 2 bytes
  unsigned long version_needed;       // version needed to extract       2 bytes
  unsigned long flag;                 // general purpose bit flag        2 bytes
  unsigned long compression_method;   // compression method              2 bytes
  unsigned long dosDate;              // last mod file date in Dos fmt   4 bytes
  unsigned long crc;                  // crc-32                          4 bytes
  unsigned long compressed_size;      // compressed size                 4 bytes
  unsigned long uncompressed_size;    // uncompressed size               4 bytes
  unsigned long size_filename;        // filename length                 2 bytes
  unsigned long size_file_extra;      // extra field length              2 bytes
  unsigned long size_file_comment;    // file comment length             2 bytes
  unsigned long disk_num_start;       // disk number start               2 bytes
  unsigned long internal_fa;          // internal file attributes        2 bytes
  unsigned long external_fa;          // external file attributes        4 bytes
  tm_unz tmu_date;
} unz_file_info;

#define UNZ_OK                  (0)
#define UNZ_END_OF_LIST_OF_FILE (-100)
#define UNZ_ERRNO               (Z_ERRNO)
#define UNZ_EOF                 (0)
#define UNZ_PARAMERROR          (-102)
#define UNZ_BADZIPFILE          (-103)
#define UNZ_INTERNALERROR       (-104)
#define UNZ_CRCERROR            (-105)
#define UNZ_PASSWORD            (-106)


// case sensitivity when searching for filenames
#define CASE_SENSITIVE 1
#define CASE_INSENSITIVE 2

// Basic data types
typedef unsigned char  Byte;  // 8 bits
typedef unsigned int   uInt;  // 16 bits or more
typedef unsigned long  uLong; // 32 bits or more
typedef void *voidpf;
typedef void *voidp;

typedef voidpf (*alloc_func) (voidpf opaque, uInt items, uInt size);
typedef void   (*free_func)  (voidpf opaque, voidpf address);

struct internal_state;

typedef unsigned char  uch;
typedef uch uchf;
typedef unsigned short ush;
typedef ush ushf;
typedef unsigned long  ulg;

// Diagnostic functions
#undef Assert
#undef Trace
#undef Tracev
#undef Tracevv
#undef Tracec
#undef Tracecv

#ifdef DEBUG

int z_verbose = 0;
void z_error (char *m) {fprintf(stderr, "%s\n", m); exit(1);}

#define Assert(cond,msg) {if(!(cond)) z_error(msg);}
#define Trace(x) {if (z_verbose>=0) fprintf x ;}
#define Tracev(x) {if (z_verbose>0) fprintf x ;}
#define Tracevv(x) {if (z_verbose>1) fprintf x ;}
#define Tracec(c,x) {if (z_verbose>0 && (c)) fprintf x ;}
#define Tracecv(c,x) {if (z_verbose>1 && (c)) fprintf x ;}

#else

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ((void)0)
#endif
#endif

#define Assert(cond,msg)	__noop
#define Trace(x)			__noop
#define Tracev(x)			__noop
#define Tracevv(x)			__noop
#define Tracec(c,x)			__noop
#define Tracecv(c,x)		__noop

#endif

// unzip.c -- IO on .zip files using zlib
// Version 0.15 beta, Mar 19th, 1998,
// Read unzip.h for more info




#define UNZ_BUFSIZE (16384)
#define UNZ_MAXFILENAMEINZIP (256)
#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)

const char unz_copyright[] = " unzip 0.15 Copyright 1998 Gilles Vollant ";

// unz_file_info_interntal contain internal info about a file in zipfile
typedef struct unz_file_info_internal_s
{
    uLong offset_curfile;// relative offset of local header 4 bytes
} unz_file_info_internal;


typedef struct
{ bool is_handle; // either a handle or memory
  bool canseek;
  // for handles:
  HANDLE h; bool herr; unsigned long initial_offset; bool mustclosehandle;
  // for memory:
  void *buf; unsigned int len,pos; // if it's a memory block
} LUFILE;


LUFILE *lufopen(void *z,unsigned int len,DWORD flags,ZRESULT *err)
{ 
if (flags!=ZIP_HANDLE && flags!=ZIP_FILENAME && flags!=ZIP_MEMORY) 
{
*err=ZR_ARGS;
 return NULL;
}
  //
  HANDLE h=0; bool canseek=false; *err=ZR_OK;
  bool mustclosehandle=false;
  if (flags==ZIP_HANDLE||flags==ZIP_FILENAME)
  { 
if (flags==ZIP_HANDLE)
    { 
HANDLE hf = (HANDLE)z;
      h=hf; 
mustclosehandle=false;
      BOOL res = DuplicateHandle(GetCurrentProcess(),hf,GetCurrentProcess(),&h,0,FALSE,DUPLICATE_SAME_ACCESS);
      if (!res) mustclosehandle=true;
    }
    else
    {
      h=CreateFile((const TCHAR*)z,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
      if (h==INVALID_HANDLE_VALUE) {*err=ZR_NOFILE; return NULL;}
      mustclosehandle=true;
    }
    // test if we can seek on it. We can't use GetFileType(h)==FILE_TYPE_DISK since it's not on CE.
    DWORD res = GetFilePosU(h);
    canseek = (res!=0xFFFFFFFF);
  }
  LUFILE *lf = new LUFILE;
  if (flags==ZIP_HANDLE||flags==ZIP_FILENAME)
  { lf->is_handle=true; lf->mustclosehandle=mustclosehandle;
    lf->canseek=canseek;
    lf->h=h; lf->herr=false;
    lf->initial_offset=0;
    if (canseek) lf->initial_offset = GetFilePosU(h);
  }
  else
  { lf->is_handle=false;
    lf->canseek=true;
    lf->mustclosehandle=false;
    lf->buf=z; lf->len=len; lf->pos=0; lf->initial_offset=0;
  }
  *err=ZR_OK;
  return lf;
}


int lufclose(LUFILE *stream)
{ if (stream==NULL) return EOF;
  if (stream->mustclosehandle) CloseHandle(stream->h);
  delete stream;
  return 0;
}

int luferror(LUFILE *stream)
{ if (stream->is_handle && stream->herr) return 1;
  else return 0;
}

long int luftell(LUFILE *stream)
{ if (stream->is_handle && stream->canseek) return GetFilePosU(stream->h)-stream->initial_offset;
  else if (stream->is_handle) return 0;
  else return stream->pos;
}

int lufseek(LUFILE *stream, long offset, int whence)
{ if (stream->is_handle && stream->canseek)
  { 
    if (whence==SEEK_SET) SetFilePointer(stream->h,stream->initial_offset+offset,0,FILE_BEGIN);
    else if (whence==SEEK_CUR) SetFilePointer(stream->h,offset,NULL,FILE_CURRENT);
    else if (whence==SEEK_END) SetFilePointer(stream->h,offset,NULL,FILE_END);
    else return 19; // EINVAL
    return 0;
  }
  else if (stream->is_handle) return 29; // ESPIPE
  else
  { if (whence==SEEK_SET) stream->pos=offset;
    else if (whence==SEEK_CUR) stream->pos+=offset;
    else if (whence==SEEK_END) stream->pos=stream->len+offset;
    return 0;
  }
}


size_t lufread(void *ptr,size_t size,size_t n,LUFILE *stream)
{ unsigned int toread = (unsigned int)(size*n);
  if (stream->is_handle)
  { DWORD red; BOOL res = ReadFile(stream->h,ptr,toread,&red,NULL);
    if (!res) stream->herr=true;
    return red/size;
  }
  if (stream->pos+toread > stream->len) toread = stream->len-stream->pos;
  memcpy(ptr, (char*)stream->buf + stream->pos, toread); DWORD red = toread;
  stream->pos += red;
  return red/size;
}




// file_in_zip_read_info_s contain internal information about a file in zipfile,
//  when reading and decompress it
typedef struct
{
	char  *read_buffer;         // internal buffer for compressed data
	z_stream stream;            // zLib stream structure for inflate

	uLong pos_in_zipfile;       // position in byte on the zipfile, for fseek
	uLong stream_initialised;   // flag set if stream structure is initialised

	uLong offset_local_extrafield;// offset of the local extra field
	uInt  size_local_extrafield;// size of the local extra field
	uLong pos_local_extrafield;   // position in the local extra field in read

	uLong crc32;                // crc32 of all data uncompressed
	uLong crc32_wait;           // crc32 we must obtain after decompress all
	uLong rest_read_compressed; // number of byte to be decompressed
	uLong rest_read_uncompressed;//number of byte to be obtained after decomp
	LUFILE* file;                 // io structore of the zipfile
	uLong compression_method;   // compression method (0==store)
	uLong byte_before_the_zipfile;// byte before the zipfile, (>0 for sfx)
	bool encrypted;               // is it encrypted?
	unsigned long keys[3];        // decryption keys, initialized by unzOpenCurrentFile
	int encheadleft;              // the first call(s) to unzReadCurrentFile will read this many encryption-header bytes first
	char crcenctest;              // if encrypted, we'll check the encryption buffer against this
} file_in_zip_read_info_s;


// unz_s contain internal information about the zipfile
typedef struct
{
	LUFILE* file;               // io structore of the zipfile
	unz_global_info gi;         // public global information
	uLong byte_before_the_zipfile;// byte before the zipfile, (>0 for sfx)
	uLong num_file;             // number of the current file in the zipfile
	uLong pos_in_central_dir;   // pos of the current file in the central dir
	uLong current_file_ok;      // flag about the usability of the current file
	uLong central_pos;          // position of the beginning of the central dir

	uLong size_central_dir;     // size of the central directory
	uLong offset_central_dir;   // offset of start of central directory with respect to the starting disk number

	unz_file_info cur_file_info; // public info about the current file in zip
	unz_file_info_internal cur_file_info_internal; // private info about it
    file_in_zip_read_info_s* pfile_in_zip_read; // structure about the current file if we are decompressing it
} unz_s, *unzFile;


int unzStringFileNameCompare (const char* fileName1,const char* fileName2,int iCaseSensitivity);
//   Compare two filename (fileName1,fileName2).

z_off_t unztell (unzFile file);
//  Give the current position in uncompressed data

int unzeof (unzFile file);
//  return 1 if the end of file was reached, 0 elsewhere

int unzGetLocalExtrafield (unzFile file, voidp buf, unsigned len);
//  Read extra field from the current file (opened by unzOpenCurrentFile)
//  This is the local-header version of the extra field (sometimes, there is
//    more info in the local-header version than in the central-header)
//
//  if buf==NULL, it return the size of the local extra field
//
//  if buf!=NULL, len is the size of the buffer, the extra header is copied in
//	buf.
//  the return value is the number of bytes copied in buf, or (if <0)
//	the error code



// ===========================================================================
//   Read a byte from a gz_stream; update next_in and avail_in. Return EOF
// for end of file.
// IN assertion: the stream s has been sucessfully opened for reading.

int unzlocal_getByte(LUFILE *fin,int *pi)
{ unsigned char c;
  int err = (int)lufread(&c, 1, 1, fin);
  if (err==1)
  { *pi = (int)c;
    return UNZ_OK;
  }
  else
  { if (luferror(fin)) return UNZ_ERRNO;
    else return UNZ_EOF;
  }
}


// ===========================================================================
// Reads a long in LSB order from the given gz_stream. Sets
int unzlocal_getShort (LUFILE *fin,uLong *pX)
{
    uLong x ;
    int i;
    int err;

    err = unzlocal_getByte(fin,&i);
    x = (uLong)i;

    if (err==UNZ_OK)
        err = unzlocal_getByte(fin,&i);
    x += ((uLong)i)<<8;

    if (err==UNZ_OK)
        *pX = x;
    else
        *pX = 0;
    return err;
}

int unzlocal_getLong (LUFILE *fin,uLong *pX)
{
    uLong x ;
    int i;
    int err;

    err = unzlocal_getByte(fin,&i);
    x = (uLong)i;
    
    if (err==UNZ_OK)
        err = unzlocal_getByte(fin,&i);
    x += ((uLong)i)<<8;

    if (err==UNZ_OK)
        err = unzlocal_getByte(fin,&i);
    x += ((uLong)i)<<16;

    if (err==UNZ_OK)
        err = unzlocal_getByte(fin,&i);
    x += ((uLong)i)<<24;
   
    if (err==UNZ_OK)
        *pX = x;
    else
        *pX = 0;
    return err;
}


// My own strcmpi / strcasecmp 
int strcmpcasenosensitive_internal (const char* fileName1,const char *fileName2)
{
	for (;;)
	{
		char c1=*(fileName1++);
		char c2=*(fileName2++);
		if ((c1>='a') && (c1<='z'))
			c1 -= (char)0x20;
		if ((c2>='a') && (c2<='z'))
			c2 -= (char)0x20;
		if (c1=='\0')
			return ((c2=='\0') ? 0 : -1);
		if (c2=='\0')
			return 1;
		if (c1<c2)
			return -1;
		if (c1>c2)
			return 1;
	}
}




//
// Compare two filename (fileName1,fileName2).
// If iCaseSenisivity = 1, comparision is case sensitivity (like strcmp)
// If iCaseSenisivity = 2, comparision is not case sensitivity (like strcmpi or strcasecmp)
//
int unzStringFileNameCompare (const char*fileName1,const char*fileName2,int iCaseSensitivity)
{ if (iCaseSensitivity==1) return strcmp(fileName1,fileName2);
  else return strcmpcasenosensitive_internal(fileName1,fileName2);
} 

#define BUFREADCOMMENT (0x400)


//  Locate the Central directory of a zipfile (at the end, just before
// the global comment). Lu bugfix 2005.07.26 - returns 0xFFFFFFFF if not found,
// rather than 0, since 0 is a valid central-dir-location for an empty zipfile.
uLong unzlocal_SearchCentralDir(LUFILE *fin)
{ if (lufseek(fin,0,SEEK_END) != 0) return 0xFFFFFFFF;
  uLong uSizeFile = luftell(fin);

  uLong uMaxBack=0xffff; // maximum size of global comment
  if (uMaxBack>uSizeFile) uMaxBack = uSizeFile;

  unsigned char *buf = (unsigned char*)zmalloc(BUFREADCOMMENT+4);
  if (buf==NULL) return 0xFFFFFFFF;
  uLong uPosFound=0xFFFFFFFF;

  uLong uBackRead = 4;
  while (uBackRead<uMaxBack)
  { uLong uReadSize,uReadPos ;
    int i;
    if (uBackRead+BUFREADCOMMENT>uMaxBack) uBackRead = uMaxBack;
    else uBackRead+=BUFREADCOMMENT;
    uReadPos = uSizeFile-uBackRead ;
    uReadSize = ((BUFREADCOMMENT+4) < (uSizeFile-uReadPos)) ? (BUFREADCOMMENT+4) : (uSizeFile-uReadPos);
    if (lufseek(fin,uReadPos,SEEK_SET)!=0) break;
    if (lufread(buf,(uInt)uReadSize,1,fin)!=1) break;
    for (i=(int)uReadSize-3; (i--)>=0;)
    { if (((*(buf+i))==0x50) && ((*(buf+i+1))==0x4b) &&	((*(buf+i+2))==0x05) && ((*(buf+i+3))==0x06))
      { uPosFound = uReadPos+i;	break;
      }
    }
    if (uPosFound!=0) break;
  }
  if (buf) zfree(buf);
  return uPosFound;
}


int unzGoToFirstFile (unzFile file);
int unzCloseCurrentFile (unzFile file);

// Open a Zip file.
// If the zipfile cannot be opened (file don't exist or in not valid), return NULL.
// Otherwise, the return value is a unzFile Handle, usable with other unzip functions
unzFile unzOpenInternal(LUFILE *fin)
{ if (fin==NULL) return NULL;
  if (unz_copyright[0]!=' ') {lufclose(fin); return NULL;}

  int err=UNZ_OK;
  unz_s us={0};
  uLong central_pos=0,uL=0;
  central_pos = unzlocal_SearchCentralDir(fin);
  if (central_pos==0xFFFFFFFF) err=UNZ_ERRNO;
  if (err==UNZ_OK && lufseek(fin,central_pos,SEEK_SET)!=0) err=UNZ_ERRNO;
  // the signature, already checked
  if (err==UNZ_OK && unzlocal_getLong(fin,&uL)!=UNZ_OK) err=UNZ_ERRNO;
  // number of this disk
  uLong number_disk=0;          // number of the current dist, used for spanning ZIP, unsupported, always 0
  if (err==UNZ_OK && unzlocal_getShort(fin,&number_disk)!=UNZ_OK) err=UNZ_ERRNO;
  // number of the disk with the start of the central directory
  uLong number_disk_with_CD=0;  // number the the disk with central dir, used for spaning ZIP, unsupported, always 0
  if (err==UNZ_OK && unzlocal_getShort(fin,&number_disk_with_CD)!=UNZ_OK) err=UNZ_ERRNO;
  // total number of entries in the central dir on this disk
  if (err==UNZ_OK && unzlocal_getShort(fin,&us.gi.number_entry)!=UNZ_OK) err=UNZ_ERRNO;
  // total number of entries in the central dir
  uLong number_entry_CD=0;      // total number of entries in the central dir (same than number_entry on nospan)
  if (err==UNZ_OK && unzlocal_getShort(fin,&number_entry_CD)!=UNZ_OK) err=UNZ_ERRNO;
  if (err==UNZ_OK && ((number_entry_CD!=us.gi.number_entry) || (number_disk_with_CD!=0) || (number_disk!=0))) err=UNZ_BADZIPFILE;
  // size of the central directory
  if (err==UNZ_OK && unzlocal_getLong(fin,&us.size_central_dir)!=UNZ_OK) err=UNZ_ERRNO;
  // offset of start of central directory with respect to the starting disk number
  if (err==UNZ_OK && unzlocal_getLong(fin,&us.offset_central_dir)!=UNZ_OK) err=UNZ_ERRNO;
  // zipfile comment length
  if (err==UNZ_OK && unzlocal_getShort(fin,&us.gi.size_comment)!=UNZ_OK) err=UNZ_ERRNO;
  if (err==UNZ_OK && ((central_pos+fin->initial_offset<us.offset_central_dir+us.size_central_dir) && (err==UNZ_OK))) err=UNZ_BADZIPFILE;
  if (err!=UNZ_OK) {lufclose(fin);return NULL;}

  us.file=fin;
  us.byte_before_the_zipfile = central_pos+fin->initial_offset - (us.offset_central_dir+us.size_central_dir);
  us.central_pos = central_pos;
  us.pfile_in_zip_read = NULL;
  fin->initial_offset = 0; // since the zipfile itself is expected to handle this

  unz_s *s = (unz_s*)zmalloc(sizeof(unz_s));
  *s=us;
  unzGoToFirstFile((unzFile)s);
  return (unzFile)s;
}



//  Close a ZipFile opened with unzipOpen.
//  If there is files inside the .Zip opened with unzipOpenCurrentFile (see later),
//    these files MUST be closed with unzipCloseCurrentFile before call unzipClose.
//  return UNZ_OK if there is no problem.
int unzClose (unzFile file)
{
	unz_s* s;
	if (file==NULL)
		return UNZ_PARAMERROR;
	s=(unz_s*)file;

    if (s->pfile_in_zip_read!=NULL)
        unzCloseCurrentFile(file);

	lufclose(s->file);
	if (s) zfree(s); // unused s=0;
	return UNZ_OK;
}


//  Write info about the ZipFile in the *pglobal_info structure.
//  No preparation of the structure is needed
//  return UNZ_OK if there is no problem. 
int unzGetGlobalInfo (unzFile file,unz_global_info *pglobal_info)
{
	unz_s* s;
	if (file==NULL)
		return UNZ_PARAMERROR;
	s=(unz_s*)file;
	*pglobal_info=s->gi;
	return UNZ_OK;
}


//   Translate date/time from Dos format to tm_unz (readable more easilty)
void unzlocal_DosDateToTmuDate (uLong ulDosDate, tm_unz* ptm)
{
    uLong uDate;
    uDate = (uLong)(ulDosDate>>16);
    ptm->tm_mday = (uInt)(uDate&0x1f) ;
    ptm->tm_mon =  (uInt)((((uDate)&0x1E0)/0x20)-1) ;
    ptm->tm_year = (uInt)(((uDate&0x0FE00)/0x0200)+1980) ;

    ptm->tm_hour = (uInt) ((ulDosDate &0xF800)/0x800);
    ptm->tm_min =  (uInt) ((ulDosDate&0x7E0)/0x20) ;
    ptm->tm_sec =  (uInt) (2*(ulDosDate&0x1f)) ;
}

//  Get Info about the current file in the zipfile, with internal only info
int unzlocal_GetCurrentFileInfoInternal (unzFile file,
                                                  unz_file_info *pfile_info,
                                                  unz_file_info_internal
                                                  *pfile_info_internal,
                                                  char *szFileName,
												  uLong fileNameBufferSize,
                                                  void *extraField,
												  uLong extraFieldBufferSize,
                                                  char *szComment,
												  uLong commentBufferSize);

int unzlocal_GetCurrentFileInfoInternal (unzFile file, unz_file_info *pfile_info,
   unz_file_info_internal *pfile_info_internal, char *szFileName,
   uLong fileNameBufferSize, void *extraField, uLong extraFieldBufferSize,
   char *szComment, uLong commentBufferSize)
{
	unz_s* s;
	unz_file_info file_info;
	unz_file_info_internal file_info_internal;
	int err=UNZ_OK;
	uLong uMagic;
	long lSeek=0;

	if (file==NULL)
		return UNZ_PARAMERROR;
	s=(unz_s*)file;
	if (lufseek(s->file,s->pos_in_central_dir+s->byte_before_the_zipfile,SEEK_SET)!=0)
		err=UNZ_ERRNO;


	// we check the magic
	if (err==UNZ_OK)
		if (unzlocal_getLong(s->file,&uMagic) != UNZ_OK)
			err=UNZ_ERRNO;
		else if (uMagic!=0x02014b50)
			err=UNZ_BADZIPFILE;

	if (unzlocal_getShort(s->file,&file_info.version) != UNZ_OK)
		err=UNZ_ERRNO;

	if (unzlocal_getShort(s->file,&file_info.version_needed) != UNZ_OK)
		err=UNZ_ERRNO;

	if (unzlocal_getShort(s->file,&file_info.flag) != UNZ_OK)
		err=UNZ_ERRNO;

	if (unzlocal_getShort(s->file,&file_info.compression_method) != UNZ_OK)
		err=UNZ_ERRNO;

	if (unzlocal_getLong(s->file,&file_info.dosDate) != UNZ_OK)
		err=UNZ_ERRNO;

    unzlocal_DosDateToTmuDate(file_info.dosDate,&file_info.tmu_date);

	if (unzlocal_getLong(s->file,&file_info.crc) != UNZ_OK)
		err=UNZ_ERRNO;

	if (unzlocal_getLong(s->file,&file_info.compressed_size) != UNZ_OK)
		err=UNZ_ERRNO;

	if (unzlocal_getLong(s->file,&file_info.uncompressed_size) != UNZ_OK)
		err=UNZ_ERRNO;

	if (unzlocal_getShort(s->file,&file_info.size_filename) != UNZ_OK)
		err=UNZ_ERRNO;

	if (unzlocal_getShort(s->file,&file_info.size_file_extra) != UNZ_OK)
		err=UNZ_ERRNO;

	if (unzlocal_getShort(s->file,&file_info.size_file_comment) != UNZ_OK)
		err=UNZ_ERRNO;

	if (unzlocal_getShort(s->file,&file_info.disk_num_start) != UNZ_OK)
		err=UNZ_ERRNO;

	if (unzlocal_getShort(s->file,&file_info.internal_fa) != UNZ_OK)
		err=UNZ_ERRNO;

	if (unzlocal_getLong(s->file,&file_info.external_fa) != UNZ_OK)
		err=UNZ_ERRNO;

	if (unzlocal_getLong(s->file,&file_info_internal.offset_curfile) != UNZ_OK)
		err=UNZ_ERRNO;

	lSeek+=file_info.size_filename;
	if ((err==UNZ_OK) && (szFileName!=NULL))
	{
		uLong uSizeRead ;
		if (file_info.size_filename<fileNameBufferSize)
		{
			*(szFileName+file_info.size_filename)='\0';
			uSizeRead = file_info.size_filename;
		}
		else
			uSizeRead = fileNameBufferSize;

		if ((file_info.size_filename>0) && (fileNameBufferSize>0))
			if (lufread(szFileName,(uInt)uSizeRead,1,s->file)!=1)
				err=UNZ_ERRNO;
		lSeek -= uSizeRead;
	}


	if ((err==UNZ_OK) && (extraField!=NULL))
	{
		uLong uSizeRead ;
		if (file_info.size_file_extra<extraFieldBufferSize)
			uSizeRead = file_info.size_file_extra;
		else
			uSizeRead = extraFieldBufferSize;

		if (lSeek!=0)
			if (lufseek(s->file,lSeek,SEEK_CUR)==0)
				lSeek=0;
			else
				err=UNZ_ERRNO;
		if ((file_info.size_file_extra>0) && (extraFieldBufferSize>0))
			if (lufread(extraField,(uInt)uSizeRead,1,s->file)!=1)
				err=UNZ_ERRNO;
		lSeek += file_info.size_file_extra - uSizeRead;
	}
	else
		lSeek+=file_info.size_file_extra;


	if ((err==UNZ_OK) && (szComment!=NULL))
	{
		uLong uSizeRead ;
		if (file_info.size_file_comment<commentBufferSize)
		{
			*(szComment+file_info.size_file_comment)='\0';
			uSizeRead = file_info.size_file_comment;
		}
		else
			uSizeRead = commentBufferSize;

		if (lSeek!=0)
			if (lufseek(s->file,lSeek,SEEK_CUR)==0)
				{} // unused lSeek=0;
			else
				err=UNZ_ERRNO;
		if ((file_info.size_file_comment>0) && (commentBufferSize>0))
			if (lufread(szComment,(uInt)uSizeRead,1,s->file)!=1)
				err=UNZ_ERRNO;
		//unused lSeek+=file_info.size_file_comment - uSizeRead;
	}
	else {} //unused lSeek+=file_info.size_file_comment;

	if ((err==UNZ_OK) && (pfile_info!=NULL))
		*pfile_info=file_info;

	if ((err==UNZ_OK) && (pfile_info_internal!=NULL))
		*pfile_info_internal=file_info_internal;

	return err;
}



//  Write info about the ZipFile in the *pglobal_info structure.
//  No preparation of the structure is needed
//  return UNZ_OK if there is no problem.
int unzGetCurrentFileInfo (unzFile file, unz_file_info *pfile_info,
  char *szFileName, uLong fileNameBufferSize, void *extraField, uLong extraFieldBufferSize,
  char *szComment, uLong commentBufferSize)
{ return unzlocal_GetCurrentFileInfoInternal(file,pfile_info,NULL,szFileName,fileNameBufferSize,
      extraField,extraFieldBufferSize, szComment,commentBufferSize);
}


//  Set the current file of the zipfile to the first file.
//  return UNZ_OK if there is no problem
int unzGoToFirstFile (unzFile file)
{
	int err;
	unz_s* s;
	if (file==NULL) return UNZ_PARAMERROR;
	s=(unz_s*)file;
	s->pos_in_central_dir=s->offset_central_dir;
	s->num_file=0;
	err=unzlocal_GetCurrentFileInfoInternal(file,&s->cur_file_info,
											 &s->cur_file_info_internal,
											 NULL,0,NULL,0,NULL,0);
	s->current_file_ok = (err == UNZ_OK);
	return err;
}


//  Set the current file of the zipfile to the next file.
//  return UNZ_OK if there is no problem
//  return UNZ_END_OF_LIST_OF_FILE if the actual file was the latest.
int unzGoToNextFile (unzFile file)
{
	unz_s* s;
	int err;

	if (file==NULL)
		return UNZ_PARAMERROR;
	s=(unz_s*)file;
	if (!s->current_file_ok)
		return UNZ_END_OF_LIST_OF_FILE;
	if (s->num_file+1==s->gi.number_entry)
		return UNZ_END_OF_LIST_OF_FILE;

	s->pos_in_central_dir += SIZECENTRALDIRITEM + s->cur_file_info.size_filename +
			s->cur_file_info.size_file_extra + s->cur_file_info.size_file_comment ;
	s->num_file++;
	err = unzlocal_GetCurrentFileInfoInternal(file,&s->cur_file_info,
											   &s->cur_file_info_internal,
											   NULL,0,NULL,0,NULL,0);
	s->current_file_ok = (err == UNZ_OK);
	return err;
}


//  Try locate the file szFileName in the zipfile.
//  For the iCaseSensitivity signification, see unzStringFileNameCompare
//  return value :
//  UNZ_OK if the file is found. It becomes the current file.
//  UNZ_END_OF_LIST_OF_FILE if the file is not found
int unzLocateFile (unzFile file, const TCHAR *szFileName, int iCaseSensitivity)
{
	unz_s* s;
	int err;


	uLong num_fileSaved;
	uLong pos_in_central_dirSaved;


	if (file==NULL)
		return UNZ_PARAMERROR;

    if (_tcslen(szFileName)>=UNZ_MAXFILENAMEINZIP)
        return UNZ_PARAMERROR;

	char szFileNameA[MAX_PATH];

#ifdef _UNICODE
	GetAnsiFileName(szFileName, szFileNameA, MAX_PATH-1);
#else
	strcpy(szFileNameA, szFileName);
#endif

	s=(unz_s*)file;
	if (!s->current_file_ok)
		return UNZ_END_OF_LIST_OF_FILE;

	num_fileSaved = s->num_file;
	pos_in_central_dirSaved = s->pos_in_central_dir;

	err = unzGoToFirstFile(file);

	while (err == UNZ_OK)
	{
		char szCurrentFileName[UNZ_MAXFILENAMEINZIP+1];
		unzGetCurrentFileInfo(file,NULL,
								szCurrentFileName,sizeof(szCurrentFileName)-1,
								NULL,0,NULL,0);
		if (unzStringFileNameCompare(szCurrentFileName,szFileNameA,iCaseSensitivity)==0)
			return UNZ_OK;
		err = unzGoToNextFile(file);
	}

	s->num_file = num_fileSaved ;
	s->pos_in_central_dir = pos_in_central_dirSaved ;
	return err;
}


//  Read the local header of the current zipfile
//  Check the coherency of the local header and info in the end of central
//        directory about this file
//  store in *piSizeVar the size of extra info in local header
//        (filename and size of extra field data)
int unzlocal_CheckCurrentFileCoherencyHeader (unz_s *s,uInt *piSizeVar,
  uLong *poffset_local_extrafield, uInt  *psize_local_extrafield)
{
	uLong uMagic,uData,uFlags;
	uLong size_filename;
	uLong size_extra_field;
	int err=UNZ_OK;

	*piSizeVar = 0;
	*poffset_local_extrafield = 0;
	*psize_local_extrafield = 0;

	if (lufseek(s->file,s->cur_file_info_internal.offset_curfile + s->byte_before_the_zipfile,SEEK_SET)!=0)
		return UNZ_ERRNO;


	if (err==UNZ_OK)
		if (unzlocal_getLong(s->file,&uMagic) != UNZ_OK)
			err=UNZ_ERRNO;
		else if (uMagic!=0x04034b50)
			err=UNZ_BADZIPFILE;

	if (unzlocal_getShort(s->file,&uData) != UNZ_OK)
		err=UNZ_ERRNO;
//	else if ((err==UNZ_OK) && (uData!=s->cur_file_info.wVersion))
//		err=UNZ_BADZIPFILE;
	if (unzlocal_getShort(s->file,&uFlags) != UNZ_OK)
		err=UNZ_ERRNO;

	if (unzlocal_getShort(s->file,&uData) != UNZ_OK)
		err=UNZ_ERRNO;
	else if ((err==UNZ_OK) && (uData!=s->cur_file_info.compression_method))
		err=UNZ_BADZIPFILE;

    if ((err==UNZ_OK) && (s->cur_file_info.compression_method!=0) &&
                         (s->cur_file_info.compression_method!=Z_DEFLATED))
        err=UNZ_BADZIPFILE;

	if (unzlocal_getLong(s->file,&uData) != UNZ_OK) // date/time
		err=UNZ_ERRNO;

	if (unzlocal_getLong(s->file,&uData) != UNZ_OK) // crc
		err=UNZ_ERRNO;
	else if ((err==UNZ_OK) && (uData!=s->cur_file_info.crc) &&
		                      ((uFlags & 8)==0))
		err=UNZ_BADZIPFILE;

	if (unzlocal_getLong(s->file,&uData) != UNZ_OK) // size compr
		err=UNZ_ERRNO;
	else if ((err==UNZ_OK) && (uData!=s->cur_file_info.compressed_size) &&
							  ((uFlags & 8)==0))
		err=UNZ_BADZIPFILE;

	if (unzlocal_getLong(s->file,&uData) != UNZ_OK) // size uncompr
		err=UNZ_ERRNO;
	else if ((err==UNZ_OK) && (uData!=s->cur_file_info.uncompressed_size) &&
							  ((uFlags & 8)==0))
		err=UNZ_BADZIPFILE;


	if (unzlocal_getShort(s->file,&size_filename) != UNZ_OK)
		err=UNZ_ERRNO;
	else if ((err==UNZ_OK) && (size_filename!=s->cur_file_info.size_filename))
		err=UNZ_BADZIPFILE;

	*piSizeVar += (uInt)size_filename;

	if (unzlocal_getShort(s->file,&size_extra_field) != UNZ_OK)
		err=UNZ_ERRNO;
	*poffset_local_extrafield= s->cur_file_info_internal.offset_curfile +
									SIZEZIPLOCALHEADER + size_filename;
	*psize_local_extrafield = (uInt)size_extra_field;

	*piSizeVar += (uInt)size_extra_field;

	return err;
}





//  Open for reading data the current file in the zipfile.
//  If there is no error and the file is opened, the return value is UNZ_OK.
int unzOpenCurrentFile (unzFile file, const char *password)
{
	int err;
	int Store;
	uInt iSizeVar;
	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	uLong offset_local_extrafield;  // offset of the local extra field
	uInt  size_local_extrafield;    // size of the local extra field

	if (file==NULL)
		return UNZ_PARAMERROR;
	s=(unz_s*)file;
	if (!s->current_file_ok)
		return UNZ_PARAMERROR;

    if (s->pfile_in_zip_read != NULL)
        unzCloseCurrentFile(file);

	if (unzlocal_CheckCurrentFileCoherencyHeader(s,&iSizeVar,
				&offset_local_extrafield,&size_local_extrafield)!=UNZ_OK)
		return UNZ_BADZIPFILE;

	pfile_in_zip_read_info = (file_in_zip_read_info_s*)zmalloc(sizeof(file_in_zip_read_info_s));
	if (pfile_in_zip_read_info==NULL)
		return UNZ_INTERNALERROR;

	pfile_in_zip_read_info->read_buffer=(char*)zmalloc(UNZ_BUFSIZE);
	pfile_in_zip_read_info->offset_local_extrafield = offset_local_extrafield;
	pfile_in_zip_read_info->size_local_extrafield = size_local_extrafield;
	pfile_in_zip_read_info->pos_local_extrafield=0;

	if (pfile_in_zip_read_info->read_buffer==NULL)
	{
		if (pfile_in_zip_read_info!=0) zfree(pfile_in_zip_read_info); //unused pfile_in_zip_read_info=0;
		return UNZ_INTERNALERROR;
	}

	pfile_in_zip_read_info->stream_initialised=0;

	if ((s->cur_file_info.compression_method!=0) && (s->cur_file_info.compression_method!=Z_DEFLATED))
        { // unused err=UNZ_BADZIPFILE;
        }
	Store = s->cur_file_info.compression_method==0;

	pfile_in_zip_read_info->crc32_wait=s->cur_file_info.crc;
	pfile_in_zip_read_info->crc32=0;
	pfile_in_zip_read_info->compression_method = s->cur_file_info.compression_method;
	pfile_in_zip_read_info->file=s->file;
	pfile_in_zip_read_info->byte_before_the_zipfile=s->byte_before_the_zipfile;

    pfile_in_zip_read_info->stream.total_out = 0;

	if (!Store)
	{
	  pfile_in_zip_read_info->stream.zalloc = (alloc_func)0;
	  pfile_in_zip_read_info->stream.zfree = (free_func)0;
	  pfile_in_zip_read_info->stream.opaque = (voidpf)0;

      err=inflateInit2(&pfile_in_zip_read_info->stream, -15);
	  if (err == Z_OK)
	    pfile_in_zip_read_info->stream_initialised=1;
        // windowBits is passed < 0 to tell that there is no zlib header.
        // Note that in this case inflate *requires* an extra "dummy" byte
        // after the compressed stream in order to complete decompression and
        // return Z_STREAM_END.
        // In unzip, i don't wait absolutely Z_STREAM_END because I known the
        // size of both compressed and uncompressed data
	}
	pfile_in_zip_read_info->rest_read_compressed = s->cur_file_info.compressed_size ;
	pfile_in_zip_read_info->rest_read_uncompressed = s->cur_file_info.uncompressed_size ;
	pfile_in_zip_read_info->encrypted = (s->cur_file_info.flag&1)!=0;
	bool extlochead = (s->cur_file_info.flag&8)!=0;
	if (extlochead) pfile_in_zip_read_info->crcenctest = (char)((s->cur_file_info.dosDate>>8)&0xff);
	else pfile_in_zip_read_info->crcenctest = (char)(s->cur_file_info.crc >> 24);
	pfile_in_zip_read_info->encheadleft = (pfile_in_zip_read_info->encrypted?12:0);
	pfile_in_zip_read_info->keys[0] = 305419896L;
	pfile_in_zip_read_info->keys[1] = 591751049L;
	pfile_in_zip_read_info->keys[2] = 878082192L;
	for (const char *cp=password; cp!=0 && *cp!=0; cp++) Uupdate_keys(pfile_in_zip_read_info->keys,*cp);

	pfile_in_zip_read_info->pos_in_zipfile =
            s->cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER +
			  iSizeVar;

	pfile_in_zip_read_info->stream.avail_in = (uInt)0;

	s->pfile_in_zip_read = pfile_in_zip_read_info;

    return UNZ_OK;
}


//  Read bytes from the current file.
//  buf contain buffer where data must be copied
//  len the size of buf.
//  return the number of byte copied if somes bytes are copied (and also sets *reached_eof)
//  return 0 if the end of file was reached. (and also sets *reached_eof).
//  return <0 with error code if there is an error. (in which case *reached_eof is meaningless)
//    (UNZ_ERRNO for IO error, or zLib error for uncompress error)
int unzReadCurrentFile  (unzFile file, voidp buf, unsigned len, bool *reached_eof)
{ int err=UNZ_OK;
  uInt iRead = 0;
  if (reached_eof!=0) *reached_eof=false;

  unz_s *s = (unz_s*)file;
  if (s==NULL) return UNZ_PARAMERROR;

  file_in_zip_read_info_s* pfile_in_zip_read_info = s->pfile_in_zip_read;
  if (pfile_in_zip_read_info==NULL) return UNZ_PARAMERROR;
  if ((pfile_in_zip_read_info->read_buffer == NULL)) return UNZ_END_OF_LIST_OF_FILE;
  if (len==0) return 0;

  pfile_in_zip_read_info->stream.next_out = (Byte*)buf;
  pfile_in_zip_read_info->stream.avail_out = (uInt)len;

  if (len>pfile_in_zip_read_info->rest_read_uncompressed)
  { pfile_in_zip_read_info->stream.avail_out = (uInt)pfile_in_zip_read_info->rest_read_uncompressed;
  }

  while (pfile_in_zip_read_info->stream.avail_out>0)
  { if ((pfile_in_zip_read_info->stream.avail_in==0) && (pfile_in_zip_read_info->rest_read_compressed>0))
    { uInt uReadThis = UNZ_BUFSIZE;
      if (pfile_in_zip_read_info->rest_read_compressed<uReadThis) uReadThis = (uInt)pfile_in_zip_read_info->rest_read_compressed;
      if (uReadThis == 0) {if (reached_eof!=0) *reached_eof=true; return UNZ_EOF;}
      if (lufseek(pfile_in_zip_read_info->file, pfile_in_zip_read_info->pos_in_zipfile + pfile_in_zip_read_info->byte_before_the_zipfile,SEEK_SET)!=0) return UNZ_ERRNO;
      if (lufread(pfile_in_zip_read_info->read_buffer,uReadThis,1,pfile_in_zip_read_info->file)!=1) return UNZ_ERRNO;
      pfile_in_zip_read_info->pos_in_zipfile += uReadThis;
      pfile_in_zip_read_info->rest_read_compressed-=uReadThis;
      pfile_in_zip_read_info->stream.next_in = (Byte*)pfile_in_zip_read_info->read_buffer;
      pfile_in_zip_read_info->stream.avail_in = (uInt)uReadThis;
      //
      if (pfile_in_zip_read_info->encrypted)
      { char *buf = (char*)pfile_in_zip_read_info->stream.next_in;
        for (unsigned int i=0; i<uReadThis; i++) buf[i]=zdecode(pfile_in_zip_read_info->keys,buf[i]);
      }
    }

    unsigned int uDoEncHead = pfile_in_zip_read_info->encheadleft;
    if (uDoEncHead>pfile_in_zip_read_info->stream.avail_in) uDoEncHead=pfile_in_zip_read_info->stream.avail_in;
    if (uDoEncHead>0)
    { char bufcrc=pfile_in_zip_read_info->stream.next_in[uDoEncHead-1];
      pfile_in_zip_read_info->rest_read_uncompressed-=uDoEncHead;
      pfile_in_zip_read_info->stream.avail_in -= uDoEncHead;
      pfile_in_zip_read_info->stream.next_in += uDoEncHead;
      pfile_in_zip_read_info->encheadleft -= uDoEncHead;
      if (pfile_in_zip_read_info->encheadleft==0)
      { if (bufcrc!=pfile_in_zip_read_info->crcenctest) return UNZ_PASSWORD;
      }
    }

    if (pfile_in_zip_read_info->compression_method==0)
    { uInt uDoCopy,i ;
      if (pfile_in_zip_read_info->stream.avail_out < pfile_in_zip_read_info->stream.avail_in)
      { uDoCopy = pfile_in_zip_read_info->stream.avail_out ;
      }
      else
      { uDoCopy = pfile_in_zip_read_info->stream.avail_in ;
      }
      for (i=0;i<uDoCopy;i++) *(pfile_in_zip_read_info->stream.next_out+i) = *(pfile_in_zip_read_info->stream.next_in+i);
      pfile_in_zip_read_info->crc32 = ucrc32(pfile_in_zip_read_info->crc32,pfile_in_zip_read_info->stream.next_out,uDoCopy);
      pfile_in_zip_read_info->rest_read_uncompressed-=uDoCopy;
      pfile_in_zip_read_info->stream.avail_in -= uDoCopy;
      pfile_in_zip_read_info->stream.avail_out -= uDoCopy;
      pfile_in_zip_read_info->stream.next_out += uDoCopy;
      pfile_in_zip_read_info->stream.next_in += uDoCopy;
      pfile_in_zip_read_info->stream.total_out += uDoCopy;
      iRead += uDoCopy;
      if (pfile_in_zip_read_info->rest_read_uncompressed==0) {if (reached_eof!=0) *reached_eof=true;}
    }
    else
    { uLong uTotalOutBefore,uTotalOutAfter;
      const Byte *bufBefore;
      uLong uOutThis;
      int flush=Z_SYNC_FLUSH;
      uTotalOutBefore = pfile_in_zip_read_info->stream.total_out;
      bufBefore = pfile_in_zip_read_info->stream.next_out;
      //
      err=inflate(&pfile_in_zip_read_info->stream,flush);
      //
      uTotalOutAfter = pfile_in_zip_read_info->stream.total_out;
      uOutThis = uTotalOutAfter-uTotalOutBefore;
      pfile_in_zip_read_info->crc32 = ucrc32(pfile_in_zip_read_info->crc32,bufBefore,(uInt)(uOutThis));
      pfile_in_zip_read_info->rest_read_uncompressed -= uOutThis;
      iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);
      if (err==Z_STREAM_END || pfile_in_zip_read_info->rest_read_uncompressed==0)
      { if (reached_eof!=0) *reached_eof=true;
        return iRead;
      }
	  if (err != Z_OK) break;
    }
  }

  if (err==Z_OK) return iRead;
  return err;
}


//  Give the current position in uncompressed data
z_off_t unztell (unzFile file)
{
	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	if (file==NULL)
		return UNZ_PARAMERROR;
	s=(unz_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

	if (pfile_in_zip_read_info==NULL)
		return UNZ_PARAMERROR;

	return (z_off_t)pfile_in_zip_read_info->stream.total_out;
}


//  return 1 if the end of file was reached, 0 elsewhere
int unzeof (unzFile file)
{
	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	if (file==NULL)
		return UNZ_PARAMERROR;
	s=(unz_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

	if (pfile_in_zip_read_info==NULL)
		return UNZ_PARAMERROR;

	if (pfile_in_zip_read_info->rest_read_uncompressed == 0)
		return 1;
	else
		return 0;
}



//  Read extra field from the current file (opened by unzOpenCurrentFile)
//  This is the local-header version of the extra field (sometimes, there is
//    more info in the local-header version than in the central-header)
//  if buf==NULL, it return the size of the local extra field that can be read
//  if buf!=NULL, len is the size of the buffer, the extra header is copied in buf.
//  the return value is the number of bytes copied in buf, or (if <0) the error code
int unzGetLocalExtrafield (unzFile file,voidp buf,unsigned len)
{
	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	uInt read_now;
	uLong size_to_read;

	if (file==NULL)
		return UNZ_PARAMERROR;
	s=(unz_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

	if (pfile_in_zip_read_info==NULL)
		return UNZ_PARAMERROR;

	size_to_read = (pfile_in_zip_read_info->size_local_extrafield -
				pfile_in_zip_read_info->pos_local_extrafield);

	if (buf==NULL)
		return (int)size_to_read;

	if (len>size_to_read)
		read_now = (uInt)size_to_read;
	else
		read_now = (uInt)len ;

	if (read_now==0)
		return 0;

	if (lufseek(pfile_in_zip_read_info->file, pfile_in_zip_read_info->offset_local_extrafield +  pfile_in_zip_read_info->pos_local_extrafield,SEEK_SET)!=0)
		return UNZ_ERRNO;

	if (lufread(buf,(uInt)size_to_read,1,pfile_in_zip_read_info->file)!=1)
		return UNZ_ERRNO;

	return (int)read_now;
}

//  Close the file in zip opened with unzipOpenCurrentFile
//  Return UNZ_CRCERROR if all the file was read but the CRC is not good
int unzCloseCurrentFile (unzFile file)
{
	int err=UNZ_OK;

	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	if (file==NULL)
		return UNZ_PARAMERROR;
	s=(unz_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

	if (pfile_in_zip_read_info==NULL)
		return UNZ_PARAMERROR;


	if (pfile_in_zip_read_info->rest_read_uncompressed == 0)
	{
		if (pfile_in_zip_read_info->crc32 != pfile_in_zip_read_info->crc32_wait)
			err=UNZ_CRCERROR;
	}


	if (pfile_in_zip_read_info->read_buffer!=0)
        { void *buf = pfile_in_zip_read_info->read_buffer;
          zfree(buf);
          pfile_in_zip_read_info->read_buffer=0;
        }
	pfile_in_zip_read_info->read_buffer = NULL;
	if (pfile_in_zip_read_info->stream_initialised)
		inflateEnd(&pfile_in_zip_read_info->stream);

	pfile_in_zip_read_info->stream_initialised = 0;
        if (pfile_in_zip_read_info!=0) zfree(pfile_in_zip_read_info); // unused pfile_in_zip_read_info=0;

    s->pfile_in_zip_read=NULL;

	return err;
}


//  Get the global comment string of the ZipFile, in the szComment buffer.
//  uSizeBuf is the size of the szComment buffer.
//  return the number of byte copied or an error code <0
int unzGetGlobalComment (unzFile file, char *szComment, uLong uSizeBuf)
{ //int err=UNZ_OK;
  unz_s* s;
  uLong uReadThis ;
  if (file==NULL) return UNZ_PARAMERROR;
  s=(unz_s*)file;
  uReadThis = uSizeBuf;
  if (uReadThis>s->gi.size_comment) uReadThis = s->gi.size_comment;
  if (lufseek(s->file,s->central_pos+22,SEEK_SET)!=0) return UNZ_ERRNO;
  if (uReadThis>0)
  { *szComment='\0';
    if (lufread(szComment,(uInt)uReadThis,1,s->file)!=1) return UNZ_ERRNO;
  }
  if ((szComment != NULL) && (uSizeBuf > s->gi.size_comment)) *(szComment+s->gi.size_comment)='\0';
  return (int)uReadThis;
}





int unzOpenCurrentFile (unzFile file, const char *password);
int unzReadCurrentFile (unzFile file, void *buf, unsigned len);
int unzCloseCurrentFile (unzFile file);




class TUnzip
{ public:
  TUnzip(const char *pwd) : uf(0), unzbuf(0), currentfile(-1), czei(-1), password(0) {if (pwd!=0) {password=new char[strlen(pwd)+1]; strcpy(password,pwd);}}
  ~TUnzip() {if (password!=0) delete[] password; password=0; if (unzbuf!=0) delete[] unzbuf; unzbuf=0;}

  unzFile uf; int currentfile; ZIPENTRY cze; int czei;
  char *password;
  char *unzbuf;            // lazily created and destroyed, used by Unzip
  TCHAR rootdir[MAX_PATH]; // includes a trailing slash

  ZRESULT Open(void *z,unsigned int len,DWORD flags);
  ZRESULT Get(int index,ZIPENTRY *ze);
  ZRESULT Find(const TCHAR *name,bool ic,int *index,ZIPENTRY *ze);
  ZRESULT Unzip(int index,void *dst,unsigned int len,DWORD flags);
  ZRESULT SetUnzipBaseDir(const TCHAR *dir);
  ZRESULT Close();
};


ZRESULT TUnzip::Open(void *z,unsigned int len,DWORD flags)
{ if (uf!=0 || currentfile!=-1) return ZR_NOTINITED;
  //
	GetCurrentDirectory(MAX_PATH-1,rootdir);
	TCHAR *lastchar = &rootdir[_tcslen(rootdir)-1];
        if (*lastchar!='\\' && *lastchar!='/') {lastchar[1]='/'; lastchar[2]=0;}
  //
	if (flags==ZIP_HANDLE)
  { // test if we can seek on it. We can't use GetFileType(h)==FILE_TYPE_DISK since it's not on CE.
    DWORD res = GetFilePosU((HANDLE)z);
    bool canseek = (res!=0xFFFFFFFF);
    if (!canseek) return ZR_SEEK;
	}
	ZRESULT e; LUFILE *f = lufopen(z,len,flags,&e);
	if (f==NULL) return e;
	uf = unzOpenInternal(f);
	if (uf==0) return ZR_NOFILE;
	return ZR_OK;
}

ZRESULT TUnzip::SetUnzipBaseDir(const TCHAR *dir)
{ _tcsncpy(rootdir,dir,MAX_PATH-1);
  TCHAR *lastchar = &rootdir[_tcslen(rootdir)-1];
  if (*lastchar!='\\' && *lastchar!='/') {lastchar[1]='/'; lastchar[2]=0;}
  return ZR_OK;
}

ZRESULT TUnzip::Get(int index,ZIPENTRY *ze)
{ if (index<-1 || index>=(int)uf->gi.number_entry) return ZR_ARGS;
  if (currentfile!=-1) unzCloseCurrentFile(uf); currentfile=-1;
  if (index==czei && index!=-1) {memcpy(ze,&cze,sizeof(ZIPENTRY)); return ZR_OK;}
  if (index==-1)
  { ze->index = uf->gi.number_entry;
    ze->name[0]=0;
    ze->attr=0;
    ze->atime.dwLowDateTime=0; ze->atime.dwHighDateTime=0;
    ze->ctime.dwLowDateTime=0; ze->ctime.dwHighDateTime=0;
    ze->mtime.dwLowDateTime=0; ze->mtime.dwHighDateTime=0;
    ze->comp_size=0;
    ze->unc_size=0;
    return ZR_OK;
  }
  if (index<(int)uf->num_file) unzGoToFirstFile(uf);
  while ((int)uf->num_file<index) unzGoToNextFile(uf);
  unz_file_info ufi; char fn[MAX_PATH];
  unzGetCurrentFileInfo(uf,&ufi,fn,MAX_PATH,NULL,0,NULL,0);
  // now get the extra header. We do this ourselves, instead of
  // calling unzOpenCurrentFile &c., to avoid allocating more than necessary.
  unsigned int extralen,iSizeVar; unsigned long offset;
  int res = unzlocal_CheckCurrentFileCoherencyHeader(uf,&iSizeVar,&offset,&extralen);
  if (res!=UNZ_OK) return ZR_CORRUPT;
  if (lufseek(uf->file,offset,SEEK_SET)!=0) return ZR_READ;
  unsigned char *extra = new unsigned char[extralen];
  if (lufread(extra,1,(uInt)extralen,uf->file)!=extralen) {delete[] extra; return ZR_READ;}
  //
  ze->index=uf->num_file;
  TCHAR tfn[MAX_PATH];
#ifdef UNICODE
  MultiByteToWideChar(CP_UTF8,0,fn,-1,tfn,MAX_PATH);
#else
  strcpy(tfn,fn);
#endif
  // As a safety feature: if the zip filename had sneaky stuff
  // like "c:\windows\file.txt" or "\windows\file.txt" or "fred\..\..\..\windows\file.txt"
  // then we get rid of them all. That way, when the programmer does UnzipItem(hz,i,ze.name),
  // it won't be a problem. (If the programmer really did want to get the full evil information,
  // then they can edit out this security feature from here).
  // In particular, we chop off any prefixes that are "c:\" or "\" or "/" or "[stuff]\.." or "[stuff]/.."
  const TCHAR *sfn=tfn;
  for (;;)
  { if (sfn[0]!=0 && sfn[1]==':') {sfn+=2; continue;}
    if (sfn[0]=='\\') {sfn++; continue;}
    if (sfn[0]=='/') {sfn++; continue;}
    const TCHAR *c;
    c=_tcsstr(sfn,_T("\\..\\")); if (c!=0) {sfn=c+4; continue;}
    c=_tcsstr(sfn,_T("\\../")); if (c!=0) {sfn=c+4; continue;}
    c=_tcsstr(sfn,_T("/../")); if (c!=0) {sfn=c+4; continue;}
    c=_tcsstr(sfn,_T("/..\\")); if (c!=0) {sfn=c+4; continue;}
    break;
  }
  _tcsncpy(ze->name, sfn,MAX_PATH);


  unsigned long a = ufi.external_fa;
  // zip has an 'attribute' 32bit value. Its lower half is windows stuff
  // its upper half is standard unix stat.st_mode. We'll start trying
  // to read it in unix mode
  bool isdir  =   (a&0x40000000)!=0;
  bool readonly=  (a&0x00800000)==0;
  //bool readable=  (a&0x01000000)!=0; // unused
  //bool executable=(a&0x00400000)!=0; // unused
  bool hidden=false, system=false, archive=true;
  // but in normal hostmodes these are overridden by the lower half...
  int host = ufi.version>>8;
  if (host==0 || host==7 || host==11 || host==14)
  { readonly=  (a&0x00000001)!=0;
    hidden=    (a&0x00000002)!=0;
    system=    (a&0x00000004)!=0;
    isdir=     (a&0x00000010)!=0;
    archive=   (a&0x00000020)!=0;
  }
  readonly; hidden; system; isdir; archive;
  ze->attr=FILE_ATTRIBUTE_NORMAL;
  if (isdir) ze->attr |= FILE_ATTRIBUTE_DIRECTORY;
  if (archive) ze->attr|=FILE_ATTRIBUTE_ARCHIVE;
  if (hidden) ze->attr|=FILE_ATTRIBUTE_HIDDEN;
  if (readonly) ze->attr|=FILE_ATTRIBUTE_READONLY;
  if (system) ze->attr|=FILE_ATTRIBUTE_SYSTEM;
  ze->comp_size = ufi.compressed_size;
  ze->unc_size = ufi.uncompressed_size;
  //
  WORD dostime = (WORD)(ufi.dosDate&0xFFFF);
  WORD dosdate = (WORD)((ufi.dosDate>>16)&0xFFFF);
  FILETIME ftd = dosdatetime2filetime(dosdate,dostime);
  FILETIME ft; LocalFileTimeToFileTime(&ftd,&ft);
  ze->atime=ft; ze->ctime=ft; ze->mtime=ft;
  // the zip will always have at least that dostime. But if it also has
  // an extra header, then we'll instead get the info from that.
  unsigned int epos=0;
  while (epos+4<extralen)
  { char etype[3]; etype[0]=extra[epos+0]; etype[1]=extra[epos+1]; etype[2]=0;
    int size = extra[epos+2];
    if (strcmp(etype,"UT")!=0) {epos += 4+size; continue;}
    int flags = extra[epos+4];
    bool hasmtime = (flags&1)!=0;
    bool hasatime = (flags&2)!=0;
    bool hasctime = (flags&4)!=0;
    epos+=5;
    if (hasmtime)
    { lutime_t mtime = ((extra[epos+0])<<0) | ((extra[epos+1])<<8) |((extra[epos+2])<<16) | ((extra[epos+3])<<24);
	  epos+=4;
      ze->mtime = timet2filetime(mtime);
    }
    if (hasatime)
    { lutime_t atime = ((extra[epos+0])<<0) | ((extra[epos+1])<<8) |((extra[epos+2])<<16) | ((extra[epos+3])<<24);
      epos+=4;
      ze->atime = timet2filetime(atime);
    }
    if (hasctime)
    { lutime_t ctime = ((extra[epos+0])<<0) | ((extra[epos+1])<<8) |((extra[epos+2])<<16) | ((extra[epos+3])<<24);
      epos+=4;
      ze->ctime = timet2filetime(ctime);
    }
    break;
  }
  //
  if (extra!=0) delete[] extra;
  memcpy(&cze,ze,sizeof(ZIPENTRY)); czei=index;
  return ZR_OK;
}

ZRESULT TUnzip::Find(const TCHAR *tname,bool ic,int *index,ZIPENTRY *ze)
{ char name[MAX_PATH];
#ifdef UNICODE
  WideCharToMultiByte(CP_UTF8,0,tname,-1,name,MAX_PATH,0,0);
#else
  strcpy(name,tname);
#endif
  int res = unzLocateFile(uf,name,ic?CASE_INSENSITIVE:CASE_SENSITIVE);
	if (res!=UNZ_OK)
  { if (index!=0) *index=-1;
    if (ze!=NULL) {memset(ze,0,sizeof(ZIPENTRY)); ze->index=-1;}
		return ZR_NOTFOUND;
	}
  if (currentfile!=-1) unzCloseCurrentFile(uf); currentfile=-1;
	int i = (int)uf->num_file;
  if (index!=NULL) *index=i;
	if (ze!=NULL)
  { ZRESULT zres = Get(i,ze);
    if (zres!=ZR_OK) return zres;
	}
	return ZR_OK;
}

void EnsureDirectory(const TCHAR *rootdir, const TCHAR *dir)
{ // first check that rootdir exists. nb. rootdir has a trailing slash
  if (rootdir!=0)
  { TCHAR rd[MAX_PATH]; _tcsncpy(rd,rootdir,MAX_PATH); size_t len=_tcslen(rd);
    if (len>0 && (rd[len-1]=='/' || rd[len-1]=='\\')) rd[len-1]=0;
    if (!FileExists(rd)) CreateDirectory(rd,0);
  }
  if (*dir==0) return;
  const TCHAR *lastslash=dir, *c=lastslash;
  while (*c!=0) {if (*c=='/' || *c=='\\') lastslash=c; c++;}
  const TCHAR *name=lastslash;
  if (lastslash!=dir)
  { TCHAR tmp[MAX_PATH]; memcpy(tmp,dir,sizeof(TCHAR)*(lastslash-dir));
    tmp[lastslash-dir]=0;
    EnsureDirectory(rootdir,tmp);
    name++;
  }
  TCHAR cd[MAX_PATH]; *cd=0; if (rootdir!=0) _tcsncpy(cd,rootdir,MAX_PATH); cd[MAX_PATH-1]=0;
  size_t len=_tcslen(cd); _tcsncpy(cd+len,dir,MAX_PATH-len); cd[MAX_PATH-1]=0;
  if (!FileExists(cd))
  { CreateDirectory(cd,0);
  }
}



ZRESULT TUnzip::Unzip(int index,void *dst,unsigned int len,DWORD flags)
{ if (flags!=ZIP_MEMORY && flags!=ZIP_FILENAME && flags!=ZIP_HANDLE) return ZR_ARGS;
  if (flags==ZIP_MEMORY)
  { if (index!=currentfile)
    { if (currentfile!=-1) unzCloseCurrentFile(uf); currentfile=-1;
      if (index>=(int)uf->gi.number_entry) return ZR_ARGS;
      if (index<(int)uf->num_file) unzGoToFirstFile(uf);
      while ((int)uf->num_file<index) unzGoToNextFile(uf);
      unzOpenCurrentFile(uf,password); currentfile=index;
		}
    bool reached_eof;
    int res = unzReadCurrentFile(uf,dst,len,&reached_eof);
    if (res<=0) {unzCloseCurrentFile(uf); currentfile=-1;}
    if (reached_eof) return ZR_OK;
    if (res>0) return ZR_MORE;
    if (res==UNZ_PASSWORD) return ZR_PASSWORD;
			return ZR_FLATE;
	}
	// otherwise we're writing to a handle or a file
  if (currentfile!=-1) unzCloseCurrentFile(uf); currentfile=-1;
  if (index>=(int)uf->gi.number_entry) return ZR_ARGS;
  if (index<(int)uf->num_file) unzGoToFirstFile(uf);
  while ((int)uf->num_file<index) unzGoToNextFile(uf);
  ZIPENTRY ze; Get(index,&ze);
  // zipentry=directory is handled specially
  bool isdir = (ze.attr&FILE_ATTRIBUTE_DIRECTORY)!=0;
  if (isdir)
  { if (flags==ZIP_HANDLE) return ZR_OK; // don't do anything
    const TCHAR *dir = (const TCHAR*)dst;
    bool isabsolute = (dir[0]=='/' || dir[0]=='\\' || (dir[0]!=0 && dir[1]==':'));
    if (isabsolute) EnsureDirectory(0,dir); else EnsureDirectory(rootdir,dir);
    return ZR_OK;
  }
	// otherwise, we write the zipentry to a file/handle
  HANDLE h; TCHAR fn[MAX_PATH]; fn[0]=0;
  if (flags==ZIP_HANDLE) h=(HANDLE)dst;
	else
  { const TCHAR *ufn = (const TCHAR*)dst;
    // We'll qualify all relative names to our root dir, and leave absolute names as they are
    // ufn="zipfile.txt"  dir=""  name="zipfile.txt"  fn="c:\\currentdir\\zipfile.txt"
    // ufn="dir1/dir2/subfile.txt"  dir="dir1/dir2/"  name="subfile.txt"  fn="c:\\currentdir\\dir1/dir2/subfiles.txt"
    // ufn="\z\file.txt"  dir="\z\"  name="file.txt"  fn="\z\file.txt"
    // This might be a security risk, in the case where we just use the zipentry's name as "ufn", where
    // a malicious zip could unzip itself into c:\windows. Our solution is that GetZipItem (which
    // is how the user retrieve's the file's name within the zip) never returns absolute paths.
    const TCHAR *name=ufn; const TCHAR *c=name; while (*c!=0) {if (*c=='/' || *c=='\\') name=c+1; c++;}
    TCHAR dir[MAX_PATH]; _tcsncpy(dir,ufn,MAX_PATH); if (name==ufn) *dir=0; else dir[name-ufn]=0;
    bool isabsolute = (dir[0]=='/' || dir[0]=='\\' || (dir[0]!=0 && dir[1]==':'));
    if (isabsolute) {_tsprintf(fn,_T("%s%s"),dir,name); EnsureDirectory(0,dir);}
    else {_tsprintf(fn,_T("%s%s%s"),rootdir,dir,name); EnsureDirectory(rootdir,dir);}
    //
    h = CreateFile(fn,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,ze.attr,NULL);
	}
  if (h==INVALID_HANDLE_VALUE) return ZR_NOFILE;
  unzOpenCurrentFile(uf,password);
  if (unzbuf==0) unzbuf=new char[16384]; DWORD haderr=0;
  //  

  for (; haderr==0;)
  { bool reached_eof;
    int res = unzReadCurrentFile(uf,unzbuf,16384,&reached_eof);
    if (res==UNZ_PASSWORD) {haderr=ZR_PASSWORD; break;}
    if (res<0) {haderr=ZR_FLATE; break;}
    if (res>0) {DWORD writ; BOOL bres=WriteFile(h,unzbuf,res,&writ,NULL); if (!bres) {haderr=ZR_WRITE; break;}}
    if (reached_eof) break;
    if (res==0) {haderr=ZR_FLATE; break;}
	}
	unzCloseCurrentFile(uf);
  if (!haderr) SetFileTime(h,&ze.ctime,&ze.atime,&ze.mtime); // may fail if it was a pipe
  if (flags!=ZIP_HANDLE) CloseHandle(h);
  if (haderr!=0) return haderr;
	return ZR_OK;
}

ZRESULT TUnzip::Close()
{ if (currentfile!=-1) unzCloseCurrentFile(uf); currentfile=-1;
  if (uf!=0) unzClose(uf); uf=0;
  return ZR_OK;
}





ZRESULT lasterrorU=ZR_OK;

unsigned int FormatZipMessageU(ZRESULT code, char *buf,unsigned int len)
{ if (code==ZR_RECENT) code=lasterrorU;
  const char *msg="unknown zip result code";
  switch (code)
  { case ZR_OK: msg="Success"; break;
    case ZR_NODUPH: msg="Culdn't duplicate handle"; break;
    case ZR_NOFILE: msg="Couldn't create/open file"; break;
    case ZR_NOALLOC: msg="Failed to allocate memory"; break;
    case ZR_WRITE: msg="Error writing to file"; break;
    case ZR_NOTFOUND: msg="File not found in the zipfile"; break;
    case ZR_MORE: msg="Still more data to unzip"; break;
    case ZR_CORRUPT: msg="Zipfile is corrupt or not a zipfile"; break;
    case ZR_READ: msg="Error reading file"; break;
    case ZR_PASSWORD: msg="Correct password required"; break;
    case ZR_ARGS: msg="Caller: faulty arguments"; break;
    case ZR_PARTIALUNZ: msg="Caller: the file had already been partially unzipped"; break;
    case ZR_NOTMMAP: msg="Caller: can only get memory of a memory zipfile"; break;
    case ZR_MEMSIZE: msg="Caller: not enough space allocated for memory zipfile"; break;
    case ZR_FAILED: msg="Caller: there was a previous error"; break;
    case ZR_ENDED: msg="Caller: additions to the zip have already been ended"; break;
    case ZR_ZMODE: msg="Caller: mixing creation and opening of zip"; break;
    case ZR_NOTINITED: msg="Zip-bug: internal initialisation not completed"; break;
    case ZR_SEEK: msg="Zip-bug: trying to seek the unseekable"; break;
    case ZR_MISSIZE: msg="Zip-bug: the anticipated size turned out wrong"; break;
    case ZR_NOCHANGE: msg="Zip-bug: tried to change mind, but not allowed"; break;
    case ZR_FLATE: msg="Zip-bug: an internal error during flation"; break;
  }
  unsigned int mlen=(unsigned int)strlen(msg);
  if (buf==0 || len==0) return mlen;
  unsigned int n=mlen; if (n+1>len) n=len-1;
  strncpy(buf,msg,n); buf[n]=0;
  return mlen;
}


typedef struct
{ DWORD flag;
  TUnzip *unz;
} TUnzipHandleData;

HZIP OpenZipU(void *z,unsigned int len,DWORD flags, const char *password)
{ TUnzip *unz = new TUnzip(password);
	lasterrorU = unz->Open(z,len,flags);
  if (lasterrorU!=ZR_OK) {delete unz; return 0;}
	TUnzipHandleData *han = new TUnzipHandleData;
  han->flag=1; han->unz=unz; return (HZIP)han;
}

ZRESULT GetZipItem(HZIP hz, int index, ZIPENTRY *ze)
{ ze->index=0; *ze->name=0; ze->unc_size=0;
  if (hz==0) {lasterrorU=ZR_ARGS;return ZR_ARGS;}
	TUnzipHandleData *han = (TUnzipHandleData*)hz;
  if (han->flag!=1) {lasterrorU=ZR_ZMODE;return ZR_ZMODE;}
	TUnzip *unz = han->unz;
	lasterrorU = unz->Get(index,ze);
	return lasterrorU;
}

ZRESULT GetZipItemW(HZIP hz, int index, ZIPENTRYW *zew)
{ zew->index=0; *zew->name=0; zew->unc_size=0;
	if (hz==0) 
	{
		lasterrorU=ZR_ARGS;
		return ZR_ARGS;
	}
	TUnzipHandleData *han = (TUnzipHandleData*)hz;
	if (han->flag!=1) 
	{
		lasterrorU=ZR_ZMODE;
		return ZR_ZMODE;
	}
	TUnzip *unz = han->unz;
	ZIPENTRY ze;
	lasterrorU = unz->Get(index,&ze);
	if (lasterrorU == ZR_OK)
	{
		zew->index     = ze.index;
		zew->attr      = ze.attr;
		zew->atime     = ze.atime;
		zew->ctime     = ze.ctime;
		zew->mtime     = ze.mtime;
		zew->comp_size = ze.comp_size;
		zew->unc_size  = ze.unc_size;
#ifdef _UNICODE
		GetUnicodeFileName(ze.name, zew->name, MAX_PATH-1);
#else
		strcpy(zew->name, ze.name);
#endif
	}
	return lasterrorU;
}

ZRESULT FindZipItemA(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze)
{ if (hz==0) {lasterrorU=ZR_ARGS;return ZR_ARGS;}
  TUnzipHandleData *han = (TUnzipHandleData*)hz;
  if (han->flag!=1) {lasterrorU=ZR_ZMODE;return ZR_ZMODE;}
  TUnzip *unz = han->unz;
  lasterrorU = unz->Find(name,ic,index,ze);
  return lasterrorU;
}

ZRESULT FindZipItemW(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *zew)
{ 
	if (hz==0) 
	{
		lasterrorU=ZR_ARGS;
		return ZR_ARGS;
	}
	TUnzipHandleData *han = (TUnzipHandleData*)hz;
	if (han->flag!=1) 
	{
		lasterrorU=ZR_ZMODE;
		return ZR_ZMODE;
	}
	TUnzip *unz = han->unz;
	ZIPENTRY ze;
	lasterrorU = unz->Find(name,ic,index,&ze);
	if (lasterrorU == ZR_OK)
	{
		zew->index     = ze.index;
		zew->attr      = ze.attr;
		zew->atime     = ze.atime;
		zew->ctime     = ze.ctime;
		zew->mtime     = ze.mtime;
		zew->comp_size = ze.comp_size;
		zew->unc_size  = ze.unc_size;
#ifdef _UNICODE
		GetUnicodeFileName(ze.name, zew->name, MAX_PATH-1);
#else
		strcpy(zew->name, ze.name);
#endif
	}

	return lasterrorU;
}

ZRESULT UnzipItem(HZIP hz, int index, void *dst, unsigned int len, DWORD flags)
{ 
	if (hz==0) 
	{
		lasterrorU=ZR_ARGS;
		return ZR_ARGS;
	}
	TUnzipHandleData *han = (TUnzipHandleData*)hz;
  if (han->flag!=1) {lasterrorU=ZR_ZMODE;return ZR_ZMODE;}
	TUnzip *unz = han->unz;
	lasterrorU = unz->Unzip(index,dst,len,flags);
	return lasterrorU;
}

ZRESULT SetUnzipBaseDir(HZIP hz, const TCHAR *dir)
{ if (hz==0) {lasterrorU=ZR_ARGS;return ZR_ARGS;}
  TUnzipHandleData *han = (TUnzipHandleData*)hz;
  if (han->flag!=1) {lasterrorU=ZR_ZMODE;return ZR_ZMODE;}
  TUnzip *unz = han->unz;
  lasterrorU = unz->SetUnzipBaseDir(dir);
  return lasterrorU;
}


ZRESULT CloseZipU(HZIP hz)
{ if (hz==0) {lasterrorU=ZR_ARGS;return ZR_ARGS;}
  TUnzipHandleData *han = (TUnzipHandleData*)hz;
  if (han->flag!=1) {lasterrorU=ZR_ZMODE;return ZR_ZMODE;}
  TUnzip *unz = han->unz;
  lasterrorU = unz->Close();
  delete unz;
  delete han;
  return lasterrorU;
}

bool IsZipHandleU(HZIP hz)
{ if (hz==0) return false;
  TUnzipHandleData *han = (TUnzipHandleData*)hz;
  return (han->flag==1);
}

#endif
