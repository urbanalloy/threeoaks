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

#ifndef _DEFINED_ACB693F6_CC87_4f7f_86A6_4DD482AF9D8F
#define _DEFINED_ACB693F6_CC87_4f7f_86A6_4DD482AF9D8F
#if _MSC_VER > 1000
#pragma once
#endif

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HOOK_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HOOK_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifdef HOOK_EXPORTS
#define HOOK_API __declspec(dllexport)
#else
#define HOOK_API __declspec(dllimport)
#endif

HOOK_API void setSysCommandHook();
HOOK_API void clearSysCommandHook();
HOOK_API UINT getSysCommandMsg();

#ifdef __cplusplus
}
#endif // __cplusplus

#define UWM_SYSCOMMAND_MSG ("UWM_SYSCOMMAND_MSG-ACB693F6_CC87_4f7f_86A6_4DD482AF9D8F")

#endif // _DEFINED_ACB693F6_CC87_4f7f_86A6_4DD482AF9D8F