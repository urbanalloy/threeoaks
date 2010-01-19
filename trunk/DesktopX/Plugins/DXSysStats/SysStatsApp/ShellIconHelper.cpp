/*
 * SysStats Widget Framework
 * Copyright (C) 2002-2006 Paul Andrews
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"

#include "ShellIconHelper.h"

/*
 * ctor
 */
template <class T>
CShellIconHelper<T>::CShellIconHelper () 
{
    // Initialise internal variables to known good state
    m_bVisible = false;
    m_bTimerActive = false;
    m_TimerId = 0;
    m_wTimerDuration = 0;
    m_CurrentIconResource = 0;
    *m_CurrentText = 0;
}
    
/*
 * Show/Hide (create/destroy) the icon
 */
template <class T>
void CShellIconHelper<T>::SetShellVisible (bool bVisible)
{
    if (bVisible == true) // User wants to show the icon in the shell
    {
        if (m_bVisible == false) // Doesn't already exist?
        {
            // Create the shell, and timer (if applicable)
            CreateShell ();
        } // Otherwise, well you already have icon in the shell. :-)

        SetShellTimer (m_bTimerActive, m_wTimerDuration);

    }
    else // User wants rid of the icon
    { 
        if (m_bVisible == true)  // Is it there already?
        {
            // Destroy any running timer
            if (m_bTimerActive == true)
            {
                ::KillTimer (m_hWnd, m_TimerId);
            }
            DestroyShell (); // Get rid
        }
    }

    m_bVisible = bVisible;
}

template <class T>
void CShellIconHelper<T>::SetShellTipText (const char *TipText)
{
    // Save this text for when we update
    strncpy(m_CurrentText, TipText, sizeof(m_CurrentText));
    ShellNotify (NIM_MODIFY);
}

template <class T>
void CShellIconHelper<T>::SetShellIcon (WORD IconResource)
{
    // Save this icon resource for when we update
    m_CurrentIconResource = IconResource;
    ShellNotify (NIM_MODIFY);
}

template <class T>
void CShellIconHelper<T>::SetShellTimer (bool bEnabled, WORD wTimerDuration)
{
    if (bEnabled == true) // User wants to start a timer
    {
        if (m_bTimerActive)
        {
            ::KillTimer (m_hWnd, m_TimerId);
        }
        // Start the timer
        m_TimerId = ::SetTimer (m_hWnd, 1, wTimerDuration, NULL);
    }
    else // User wants to shut down the timer
    {
        if (m_bTimerActive)
        {
            ::KillTimer (m_hWnd, m_TimerId);
        }
    }
    m_wTimerDuration = wTimerDuration;
    m_bTimerActive = bEnabled;
}

/*
 * Called from SetShellVisible(TRUE).
 */
template <class T>
HRESULT CShellIconHelper<T>::CreateShell ()
{
    RECT rect;
    rect.left = rect.right = rect.top = rect.bottom = 0;

    // Create a hidden window (using CWindowImpl)
    HWND hWnd = Create (NULL, rect, "ShellIconHiddenWindow", WS_POPUP);

    if (hWnd != 0) // Was created?
    {
        // Add the icon into the shell
        ShellNotify (NIM_ADD);
        return S_OK;
    }
    else return HRESULT_FROM_WIN32 (GetLastError());
}
  
/*
 * Called from SetShellVisible(FALSE).
 */
template <class T>
void CShellIconHelper<T>::DestroyShell ()
{
    ShellNotify (NIM_DELETE); // Remove the icon
    if (m_hWnd != NULL)
    {
        // Get rid of the hidden window
        DestroyWindow();
    }
}

/*
 * Called from many places whenever the icon needs to be redrawn
 */
template <class T>
void CShellIconHelper<T>::ShellNotify (DWORD msg)
{
	if (m_hWnd != NULL)
	{
		NOTIFYICONDATA notifyIconData;
		::memset(&notifyIconData, 0, sizeof(notifyIconData));
		notifyIconData.cbSize = sizeof(notifyIconData);
		notifyIconData.hWnd = m_hWnd;
		notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		notifyIconData.uCallbackMessage = WM_USER;
		notifyIconData.uID = 0; 
       
		notifyIconData.hIcon = (HICON) ::LoadImage(      
			_Module.m_hInst,
			MAKEINTRESOURCE(m_CurrentIconResource),
			IMAGE_ICON,
			16,
			16,
			LR_DEFAULTCOLOR
		);

		strncpy(notifyIconData.szTip, m_CurrentText, sizeof(notifyIconData.szTip));
		::Shell_NotifyIcon (msg, &notifyIconData);
	}
}

