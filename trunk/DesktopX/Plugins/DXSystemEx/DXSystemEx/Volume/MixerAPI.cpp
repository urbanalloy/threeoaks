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

#include "stdafx.h"
#include "MixerAPI.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void MixerAPI::ZeroAll()
{
	m_HMixer = NULL;
	m_iMixerControlID = 0;
	mmr = MMSYSERR_NOERROR;
	m_dwChannels = 0;
	m_bSuccess = FALSE;
}

MixerAPI::MixerAPI(DWORD DstType, DWORD SrcType, DWORD ControlType)
{
	ZeroAll();
	if(mixerGetNumDevs() < 1) 
		return;

	mmr = mixerOpen(&m_HMixer, 0, 0, 0L, CALLBACK_NULL);
	if (mmr != MMSYSERR_NOERROR) 
		return;

	// get dwLineID
	MIXERLINE mxl;
	mxl.cbStruct = sizeof(MIXERLINE);

	// DstType
	mxl.dwComponentType = DstType;
	if (mixerGetLineInfo((HMIXEROBJ)m_HMixer, &mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR) 
		return;

	// SrcType
	if( SrcType != NO_SOURCE )
	{
		UINT nconn = mxl.cConnections;
		DWORD DstIndex = mxl.dwDestination;
		for( UINT j = 0; j < nconn; j++ )
		{
			mxl.cbStruct = sizeof( MIXERLINE );
			mxl.dwSource = j;
			mxl.dwDestination = DstIndex;
			if(mixerGetLineInfo( ( HMIXEROBJ )m_HMixer,	&mxl, MIXER_GETLINEINFOF_SOURCE ) != MMSYSERR_NOERROR) 
				return;

			if( mxl.dwComponentType == SrcType ) 
				break;
		}
	}

	// get dwControlID
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = ControlType;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mxc;
	if (mixerGetLineControls((HMIXEROBJ)m_HMixer, &mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
		return;
	
	m_iMixerControlID = mxc.dwControlID;
	m_dwChannels = mxl.cChannels;
	m_bSuccess = TRUE;
}

MixerAPI::MixerAPI(HWND hwnd, DWORD DstType, DWORD SrcType, DWORD ControlType)
{
	ZeroAll();
	if(mixerGetNumDevs() < 1)
		return;

	mmr = mixerOpen(&m_HMixer, 0, (DWORD)hwnd, 0L, CALLBACK_WINDOW);
	if (mmr != MMSYSERR_NOERROR)
		return;

	// get dwLineID
	MIXERLINE mxl;
	mxl.cbStruct = sizeof(MIXERLINE);

	// DstType
	mxl.dwComponentType = DstType;
	if (mixerGetLineInfo((HMIXEROBJ)m_HMixer, &mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
		return;

	// SrcType
	if( SrcType != NO_SOURCE )
	{
		UINT nconn = mxl.cConnections;
		DWORD DstIndex = mxl.dwDestination;
		for( UINT j = 0; j < nconn; j++ )
		{
			mxl.cbStruct = sizeof( MIXERLINE );
			mxl.dwSource = j;
			mxl.dwDestination = DstIndex;
			if(mixerGetLineInfo( ( HMIXEROBJ )m_HMixer,	&mxl, MIXER_GETLINEINFOF_SOURCE ) != MMSYSERR_NOERROR)
				return;

			if( mxl.dwComponentType == SrcType )
				break;
		}
	}

	// get dwControlID
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = ControlType;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mxc;
	if (mixerGetLineControls((HMIXEROBJ)m_HMixer, &mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR) 
		return;

	m_iMixerControlID = mxc.dwControlID;
	m_bSuccess = TRUE;
}

MixerAPI::~MixerAPI()
{
	if (m_HMixer) 
		mixerClose(m_HMixer);
}


/******************************************************************************
*
* Name:        GetControlValue
*
* Description: Get value(s) from the control
*
* Parameters:  dwResults - results from the call
*
* Returns:     length of dwResults (aka. the number of channels)
*
* Notes:       dwResults needs to get cleaned up by the calling program
*
*******************************************************************************/
DWORD MixerAPI::GetControlValue(DWORD **dwResults)
{
	if (!m_bSuccess) 
		return 0;

	m_bSuccess = FALSE;

	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED *mxcd_u = new MIXERCONTROLDETAILS_UNSIGNED[m_dwChannels];
	*dwResults = new DWORD[m_dwChannels];
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = m_iMixerControlID;
	mxcd.cChannels = m_dwChannels;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(mxcd_u);
	mxcd.paDetails = mxcd_u;

	mmr = mixerGetControlDetails((HMIXEROBJ)m_HMixer, &mxcd, 0L);
	if (MMSYSERR_NOERROR != mmr)  {
		delete [] mxcd_u;
		return 0;
	}

	m_bSuccess = TRUE;

	for( DWORD i=0; i<m_dwChannels; i++ )
	{
		(*dwResults)[i] = mxcd_u[i].dwValue;
	}

	/* cleanup */
	delete [] mxcd_u;

	return m_dwChannels;
}

/******************************************************************************
*
* Name:        GetMuteValue
*
* Description: Get value from the mute control
*
* Parameters:  dwResults - result from the call
*
* Return Value: FALSE if an error occurred, TRUE otherwise
*
*******************************************************************************/
BOOL MixerAPI::GetMuteValue(LONG *dwResult)
{
	if (!m_bSuccess) 
		return m_bSuccess;

	m_bSuccess = FALSE;

	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_BOOLEAN mxcdMute;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = m_iMixerControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(mxcdMute);
	mxcd.paDetails = &mxcdMute;

	mmr = mixerGetControlDetails((HMIXEROBJ)m_HMixer, &mxcd, 0L);
	if (MMSYSERR_NOERROR != mmr) 
		return m_bSuccess;

	m_bSuccess = TRUE;

	*dwResult = mxcdMute.fValue;

	return m_bSuccess;
}
/******************************************************************************
*
* Name:        SetControlValue
*
* Description: Set value(s) of the control
*
* Parameters:  dwData - data to send to the control
*              dwDataLen - length of the data
*
* Returns:     success/failure
*
* Notes:       if dwDataLen == 1, it will force the # of channels to be set to
*              1 - this is to make mute work ( aka. On(), Off() )
*
*******************************************************************************/
BOOL MixerAPI::SetControlValue(DWORD *dwData, DWORD dwDataLen)
{
	if (!m_bSuccess) 
		return m_bSuccess;

	if (dwDataLen > m_dwChannels)
		return m_bSuccess;

	m_bSuccess = FALSE;
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED *mxcd_u = new MIXERCONTROLDETAILS_UNSIGNED[m_dwChannels];
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = m_iMixerControlID;

	/* XXX - not sure this is always the case */
	mxcd.cChannels = dwDataLen==1 ? 1 : m_dwChannels;

	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(mxcd_u);
	mxcd.paDetails = mxcd_u;

	mmr = mixerGetControlDetails((HMIXEROBJ)m_HMixer, &mxcd, 0L);
	if (MMSYSERR_NOERROR != mmr)
		goto cleanup;

	for( DWORD i=0; i<dwDataLen; i++ )
	{
		mxcd_u[i].dwValue = *(dwData+i);
	}

	mmr = mixerSetControlDetails((HMIXEROBJ)m_HMixer, &mxcd, 0L);
	if (MMSYSERR_NOERROR != mmr)
		return m_bSuccess;

	m_bSuccess = TRUE;

cleanup:
	/* cleanup */
	delete [] mxcd_u;

	return m_bSuccess;
}

BOOL MixerAPI::On()
{
	DWORD tmp = 0;
	return SetControlValue(&tmp);
}

BOOL MixerAPI::Off()
{
	DWORD tmp = 1;
	return SetControlValue(&tmp);
}

