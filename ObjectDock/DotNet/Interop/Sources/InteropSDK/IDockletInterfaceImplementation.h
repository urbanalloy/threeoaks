///////////////////////////////////////////////////////////////////////////////////////////////
//
// DotNetDocklets : bringing .NET to ObjectDock
//
// Copyright (c) 2004-2009, Julien Templier
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

//
// interface IDockletInterface wrapper method implementations
//

inline HRESULT IDockletInterface::OnGetInformation ( BSTR * name, BSTR * author, long * version, BSTR * notes ) {
    HRESULT _hr = raw_OnGetInformation(name, author, version, notes);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _hr;
}

inline HRESULT IDockletInterface::OnCreate ( struct DOCKLET_STATIC_DATA data, _bstr_t ini, _bstr_t iniGroup ) {
    HRESULT _hr = raw_OnCreate(data, ini, iniGroup);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _hr;
}

inline HRESULT IDockletInterface::OnSave ( _bstr_t ini, _bstr_t iniGroup, VARIANT_BOOL isForExport ) {
    HRESULT _hr = raw_OnSave(ini, iniGroup, isForExport);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _hr;
}

inline HRESULT IDockletInterface::OnDestroy ( ) {
    HRESULT _hr = raw_OnDestroy();
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _hr;
}

inline SAFEARRAY * IDockletInterface::OnExportFiles ( ) {
    SAFEARRAY * _result = 0;
    HRESULT _hr = raw_OnExportFiles(&_result);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _result;
}

inline VARIANT_BOOL IDockletInterface::OnLeftButtonClick ( struct System_Drawing::Point * ptCursor, struct System_Drawing::Size * sizeDocklet ) {
    VARIANT_BOOL _result = 0;
    HRESULT _hr = raw_OnLeftButtonClick(ptCursor, sizeDocklet, &_result);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _result;
}

inline VARIANT_BOOL IDockletInterface::OnDoubleClick ( struct System_Drawing::Point * ptCursor, struct System_Drawing::Size * sizeDocklet ) {
    VARIANT_BOOL _result = 0;
    HRESULT _hr = raw_OnDoubleClick(ptCursor, sizeDocklet, &_result);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _result;
}

inline VARIANT_BOOL IDockletInterface::OnLeftButtonHeld ( struct System_Drawing::Point * ptCursor, struct System_Drawing::Size * sizeDocklet ) {
    VARIANT_BOOL _result = 0;
    HRESULT _hr = raw_OnLeftButtonHeld(ptCursor, sizeDocklet, &_result);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _result;
}

inline VARIANT_BOOL IDockletInterface::OnRightButtonClick ( struct System_Drawing::Point * ptCursor, struct System_Drawing::Size * sizeDocklet ) {
    VARIANT_BOOL _result = 0;
    HRESULT _hr = raw_OnRightButtonClick(ptCursor, sizeDocklet, &_result);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _result;
}

inline VARIANT_BOOL IDockletInterface::OnConfigure ( ) {
    VARIANT_BOOL _result = 0;
    HRESULT _hr = raw_OnConfigure(&_result);
	if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _result;
}

inline VARIANT_BOOL IDockletInterface::OnAcceptDropFiles ( ) {
    VARIANT_BOOL _result = 0;
    HRESULT _hr = raw_OnAcceptDropFiles(&_result);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _result;
}

inline HRESULT IDockletInterface::OnDropFiles ( long hDrop ) {
    HRESULT _hr = raw_OnDropFiles(hDrop);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _hr;
}

inline HRESULT IDockletInterface::OnProcessMessage ( long hwnd, unsigned long uMsg, long wParam, long lParam ) {
    HRESULT _hr = raw_OnProcessMessage(hwnd, uMsg, wParam, lParam);
    if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
    return _hr;
}
