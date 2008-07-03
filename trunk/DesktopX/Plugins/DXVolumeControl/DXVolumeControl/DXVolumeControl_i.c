

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0499 */
/* at Mon Feb 26 06:40:52 2007
 */
/* Compiler settings for .\DXVolumeControl.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IVistaVolume,0x972CCBF6,0x9B3A,0x4019,0x97,0x4E,0x2C,0x47,0xED,0x4C,0xF7,0xB3);


MIDL_DEFINE_GUID(IID, LIBID_DXVolumeControlLib,0x6444B45E,0x8B01,0x4F6C,0x88,0xE0,0xC9,0x54,0xA0,0x17,0xC7,0xC4);


MIDL_DEFINE_GUID(IID, DIID__IVolumeEvents,0xC4DAD7A8,0x7750,0x4FE9,0xBF,0xA8,0x29,0x33,0x64,0xE9,0x81,0x34);


MIDL_DEFINE_GUID(CLSID, CLSID_VistaVolume,0xF36B9A47,0xAF5B,0x4AE2,0xA0,0x39,0x48,0xCC,0x18,0xFE,0xEA,0x4C);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



