///////////////////////////////////////////////////////////////////////////////////////////////
//
// Dream SDK
//
// Copyright (c) 2007-2008, Julien Templier
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
//	1. Redistributions of source code must retain the above copyright notice, this list of
//		 conditions and the following disclaimer. 
//	2. Redistributions in binary form must reproduce the above copyright notice, this list
//		 of conditions and the following disclaimer in the documentation and/or other materials
//		 provided with the distribution. 
//	3. The name of the author may not be used to endorse or promote products derived from this
//		 software without specific prior written permission. 
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//	COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//	POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////////////////




// Registry
#define SAVE_KEY(name, var) \
	val = var; \
	RegSetValueEx(key, name, NULL, REG_DWORD, (CONST BYTE*)&val, sizeof(val));

#define LOAD_KEY(name, var)\
	size = sizeof(val); \
	if (RegQueryValueEx(key, name, NULL, NULL, (LPBYTE)&val, &size) == ERROR_SUCCESS) \
		var = val;

#define SAVE_KEY_FLOAT(name, var) \
	sprintf((char*)&buffer, "%.2f", var); \
	RegSetValueEx(key, name, NULL, REG_SZ, (CONST BYTE*)&buffer, sizeof(buffer));

#define LOAD_KEY_FLOAT(name, var) \
	size = sizeof(buffer); \
	strcpy((char*)&buffer, ""); \
	if (RegQueryValueEx(key, name, NULL, &type, (LPBYTE)&buffer, &size) == ERROR_SUCCESS) \
		var = (float)atof((char *)&buffer);

#define SAVE_KEY_STRING(name, var) \
	sprintf((char*)&buffer, "%s", var); \
	RegSetValueEx(key, name, NULL, REG_SZ, (CONST BYTE*)&buffer, sizeof(buffer));

#define LOAD_KEY_STRING(name, var) \
	size = sizeof(buffer); \
	strcpy((char*)&buffer, ""); \
	if (RegQueryValueEx(key, name, NULL, &type, (LPBYTE)&buffer, &size) == ERROR_SUCCESS) \
		strcpy_s((char *)var, size, (const char *)&buffer); 