'///////////////////////////////////////////////////////////////////////////////////////////////
'//
'// DXTaskbar7 Defines
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

' Errors
Const TASKBAR_INDEX_SIZE_ERR       = 1
Const TASKBAR_PATH_NOT_FOUND_ERR   = 8

' Progress State
Const PROGRESS_STATE_NOPROGRESS    = &H00000000
Const PROGRESS_STATE_INDETERMINATE = &H00000001
Const PROGRESS_STATE_NORMAL        = &H00000002 
Const PROGRESS_STATE_ERROR         = &H00000004 
Const PROGRESS_STATE_PAUSED        = &H00000008 

' Button Flags
Const BUTTON_FLAG_ENABLED        = &H00000000
Const BUTTON_FLAG_DISABLED       = &H00000001
Const BUTTON_FLAG_DISMISSONCLICK = &H00000002
Const BUTTON_FLAG_NOBACKGROUND   = &H00000004
Const BUTTON_FLAG_HIDDEN         = &H00000008
Const BUTTON_FLAG_NONINTERACTIVE = &H00000010
