'///////////////////////////////////////////////////////////////////////////////////////////////
'//
'// DXSystemEx Defines
'//
'// Copyright (c) 2009, Julien TEMPLIER
'// All rights reserved.
'//
'///////////////////////////////////////////////////////////////////////////////////////////////
'// * $LastChangedRevision$
'// * $LastChangedDate$
'// * $LastChangedBy$
'///////////////////////////////////////////////////////////////////////////////////////////////
'//
'// Redistribution and use in source and binary forms, with or without modification, are
'// permitted provided that the following conditions are met:
'//  1. Redistributions of source code must retain the above copyright notice, this list of
'//     conditions and the following disclaimer.
'//  2. Redistributions in binary form must reproduce the above copyright notice, this list
'//     of conditions and the following disclaimer in the documentation and/or other materials
'//     provided with the distribution.
'//  3. The name of the author may not be used to endorse or promote products derived from this
'//     software without specific prior written permission.
'//
'//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
'//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
'//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
'//  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
'//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
'//  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
'//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
'//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
'//  POSSIBILITY OF SUCH DAMAGE.
'//
'///////////////////////////////////////////////////////////////////////////////////////////////

'=============================================
'= Errors
'=============================================
Const ERR_INDEX_SIZE                    = 1
Const ERR_DOMSTRING_SIZE                = 2
Const ERR_HIERARCHY_REQUEST             = 3
Const ERR_WRONG_DOCUMENT                = 4
Const ERR_INVALID_CHARACTER             = 5
Const ERR_NO_DATA_ALLOWED               = 6
Const ERR_NO_MODIFICATION_ALLOWED       = 7
Const ERR_NOT_FOUND                     = 8
Const ERR_NOT_SUPPORTED                 = 9
Const ERR_INUSE_ATTRIBUTE               = 10
Const ERR_INVALID_STATE                 = 11
Const ERR_SYNTAX                        = 12
Const ERR_INVALID_MODIFICATION          = 13
Const ERR_NAMESPACE                     = 14
Const ERR_INVALID_ACCESS                = 15
Const ERR_VALIDATION                    = 16
Const ERR_TYPE_MISMATCH                 = 17

'=============================================
'= Signature
'=============================================
Const SIGNATURE_SHA1                    = 0

'=============================================
'= Archive (1.1 only)
'=============================================
Const ARCHIVE_ZIP                       = 1

'=============================================
'= Download (1.1 only)
'=============================================
Const DOWNLOAD_OK                       = 0
Const DOWNLOAD_CANCELLED                = 1
'           Connection / Request
Const DOWNLOAD_INVALIDURL               = 2
Const DOWNLOAD_CANNOTCONNECT            = 3
Const DOWNLOAD_CONNECTIONERROR          = 4
Const DOWNLOAD_NAMENOTRESOLVED          = 5
Const DOWNLOAD_SECUREFAILURE            = 6
Const DOWNLOAD_TIMEOUT                  = 7
Const DOWNLOAD_HTTPERROR                = 8
Const DOWNLOAD_READERROR                = 9
'           Download Folder
Const DOWNLOAD_INVALIDPAT               = 50
Const DOWNLOAD_FOLDERDONOTEXISTS        = 51
Const DOWNLOAD_FOLDERNOTWRITABLE        = 52
Const DOWNLOAD_FILENOTWRITABLE          = 53
'           Internal Errors
Const DOWNLOAD_INTERNALERROR            = 100
Const DOWNLOAD_CONNECTIONOPENFAILED     = 101
Const DOWNLOAD_REQUESTOPENFAILED        = 102
Const DOWNLOAD_REQUESTSENDFAILED        = 103
Const DOWNLOAD_QUERYHEADERSFAILED       = 104
