///////////////////////////////////////////////////////////////////////////////////////////////
//
// MixerAPI (CAlexfMixer) - simple mixer control wrapper
//
// Copyright (c)1999, Alexander Fedorov 
// You may do whatever you want with this code, as long as you include this
// copyright notice in your implementation files.
// If you wish to add new classes to this collection, feel free to do so.
// But please send me your code so that I can update the collection.
// Comments and bug reports: lamer2000@usa.net
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <mmsystem.h>

// Thanks to Langis Pitre
#define NO_SOURCE ((MIXERLINE_COMPONENTTYPE_SRC_LAST + 1))

class MixerAPI
{
	protected:
		HMIXER m_HMixer;
		INT m_iMixerControlID;
		MMRESULT mmr;
		DWORD m_dwChannels;
		BOOL m_bSuccess;
		void ZeroAll();

	public:
		BOOL IsOk() {return m_bSuccess;};
		BOOL On();
		BOOL Off();
		DWORD GetControlValue( DWORD **dwResults );
		BOOL GetMuteValue(LONG *dwResult);
		BOOL SetControlValue(DWORD *dw, DWORD num = 1);
		MixerAPI(DWORD DstType, DWORD SrcType, DWORD ControlType);
		MixerAPI(HWND hwnd, DWORD DstType, DWORD SrcType, DWORD ControlType);
		virtual ~MixerAPI();
};
