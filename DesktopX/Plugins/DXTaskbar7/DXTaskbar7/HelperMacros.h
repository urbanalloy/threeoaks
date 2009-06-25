///////////////////////////////////////////////////////////////////////////////////////////////
//
// Helper macros
//
// Copyright (c) 2009, Julien Templier
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer. 
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution. 
//  3. The name of the author may not be used to endorse or promote products derived from this
//     software without specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////
// Macros
#define SAFE_DELETE(_p)			{ if(_p) { delete _p;		_p=NULL; } }
#define SAFE_DELETE_ARRAY(_p)	{ if(_p) { delete [] _p;	_p=NULL; } }
#define SAFE_RELEASE(_p)		{ if(_p) { _p->Release();	_p=NULL; } }

#define ACQUIRE_MUTEX(mutex) \
	DWORD dwWaitMutex = WaitForSingleObject(mutex, INFINITE); \
	switch (dwWaitMutex) \
{ \
	case WAIT_OBJECT_0: \
{

#define RELEASE_MUTEX(mutex) \
	ReleaseMutex(mutex); \
	break; \
} \
	default: \
	break; \
}

#define EXIT_ON_ERROR(hres)\
	if (FAILED(hres)) { goto Exit; }

#define TO_I4_VARIANT(var, value)\
	var->vt = VT_I4;\
	var->lVal = value;