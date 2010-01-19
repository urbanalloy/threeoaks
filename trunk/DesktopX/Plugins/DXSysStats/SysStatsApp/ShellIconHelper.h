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


// Shell Icon Helper - by Jon Taylor, for Code Project
// See documentation on Code Project web site for information on how 
// to use this class.

#ifndef __SHELLICONHELPER_H
#define __SHELLICONHELPER_H

#include <atlwin.h>
#include <string>

template <typename T>
class CShellIconHelper : public CWindowImpl<T>
{
public:

	/*
	 * ctor
	 */
    CShellIconHelper();
    
	/*
	 * Show/Hide (create/destroy) the icon
	 */
    virtual void SetShellVisible (bool bVisible = true);

	//------------- Mutators --------------//
    virtual void SetShellTipText(const char *TipText);
    virtual void SetShellIcon(WORD IconResource);
    virtual void SetShellTimer(bool bEnabled, WORD wTimerDuration = 1000);
    virtual int  showShellMenu() = 0;

private:
	/*
	 * Called from SetShellVisible(TRUE).
	 */
    virtual HRESULT CreateShell();
      
	/*
	 * Called from SetShellVisible(FALSE).
	 */
    virtual void DestroyShell();

	/*
	 * Called from many places whenever the icon needs to be redrawn
	 */
    void ShellNotify(DWORD msg);

	// Private instance variables
    bool m_bVisible;
    bool m_bTimerActive;
    UINT_PTR m_TimerId;
    WORD    m_wTimerDuration;

    int m_CurrentIconResource;
    char m_CurrentText[64];
};

#endif


