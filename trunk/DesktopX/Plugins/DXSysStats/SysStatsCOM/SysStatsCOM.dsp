# Microsoft Developer Studio Project File - Name="SysStatsCOM" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SysStatsCOM - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SysStatsCOM.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SysStatsCOM.mak" CFG="SysStatsCOM - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SysStatsCOM - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SysStatsCOM - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SysStatsCOM - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SysStatsCOM - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SysStatsCOM - Win32 Unicode Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SysStatsCOM - Win32 Unicode Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SysStatsCOM - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SysStatsCOM - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SysStatsCommon" /D "_ATL_DLL" /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D _WIN32_WINNT=0x0500 /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /Oicf
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib shlwapi.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /out:"../SysStatsApp/Debug/SysStatsCOM.dll" /pdbtype:sept
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=\Program Files\Microsoft Visual Studio\MyProjects\SysStatsApp\Debug\SysStatsCOM.dll
InputPath=\Program Files\Microsoft Visual Studio\MyProjects\SysStatsApp\Debug\SysStatsCOM.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SysStatsCOM - Win32 Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugU"
# PROP BASE Intermediate_Dir "DebugU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SysStatsCommon" /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D _WIN32_WINNT=0x500 /D WINVER=0x500 /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# Begin Custom Build - Performing registration
OutDir=.\DebugU
TargetPath=.\DebugU\SysStatsCOM.dll
InputPath=.\DebugU\SysStatsCOM.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SysStatsCOM - Win32 Release MinSize"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinSize"
# PROP BASE Intermediate_Dir "ReleaseMinSize"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinSize"
# PROP Intermediate_Dir "ReleaseMinSize"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "..\SysStatsCommon" /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D _WIN32_WINNT=0x500 /D WINVER=0x500 /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinSize
TargetPath=.\ReleaseMinSize\SysStatsCOM.dll
InputPath=.\ReleaseMinSize\SysStatsCOM.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SysStatsCOM - Win32 Release MinDependency"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinDependency"
# PROP BASE Intermediate_Dir "ReleaseMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinDependency"
# PROP Intermediate_Dir "ReleaseMinDependency"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "..\SysStatsCommon" /D "NDEBUG" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D _WIN32_WINNT=0x500 /D WINVER=0x500 /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinDependency
TargetPath=.\ReleaseMinDependency\SysStatsCOM.dll
InputPath=.\ReleaseMinDependency\SysStatsCOM.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SysStatsCOM - Win32 Unicode Release MinSize"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinSize"
# PROP BASE Intermediate_Dir "ReleaseUMinSize"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinSize"
# PROP Intermediate_Dir "ReleaseUMinSize"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "..\SysStatsCommon" /D "NDEBUG" /D "_UNICODE" /D "_ATL_DLL" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D _WIN32_WINNT=0x500 /D WINVER=0x500 /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Performing registration
OutDir=.\ReleaseUMinSize
TargetPath=.\ReleaseUMinSize\SysStatsCOM.dll
InputPath=.\ReleaseUMinSize\SysStatsCOM.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SysStatsCOM - Win32 Unicode Release MinDependency"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinDependency"
# PROP BASE Intermediate_Dir "ReleaseUMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinDependency"
# PROP Intermediate_Dir "ReleaseUMinDependency"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "..\SysStatsCommon" /D "NDEBUG" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D _WIN32_WINNT=0x500 /D WINVER=0x500 /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Performing registration
OutDir=.\ReleaseUMinDependency
TargetPath=.\ReleaseUMinDependency\SysStatsCOM.dll
InputPath=.\ReleaseUMinDependency\SysStatsCOM.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SysStatsCOM - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SysStatsCOM___Win32_Release"
# PROP BASE Intermediate_Dir "SysStatsCOM___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O1 /I "..\SysStatsCommon" /D "NDEBUG" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D _WIN32_WINNT=0x500 /D WINVER=0x500 /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\SysStatsCommon" /D "NDEBUG" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D _WIN32_WINNT=0x500 /D WINVER=0x500 /Yu"stdafx.h" /FD /c
# ADD MTL /Oicf
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib shlwapi.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Performing registration
OutDir=.\Release
TargetPath=.\Release\SysStatsCOM.dll
InputPath=.\Release\SysStatsCOM.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "SysStatsCOM - Win32 Debug"
# Name "SysStatsCOM - Win32 Unicode Debug"
# Name "SysStatsCOM - Win32 Release MinSize"
# Name "SysStatsCOM - Win32 Release MinDependency"
# Name "SysStatsCOM - Win32 Unicode Release MinSize"
# Name "SysStatsCOM - Win32 Unicode Release MinDependency"
# Name "SysStatsCOM - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AppConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\CClientModel.cpp
# End Source File
# Begin Source File

SOURCE=.\CCompositeOverlay.cpp
# End Source File
# Begin Source File

SOURCE=.\CControllerFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\CMeterDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CMeterFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\CMeterHome.cpp
# End Source File
# Begin Source File

SOURCE=.\COMINIFile.cpp
# End Source File
# Begin Source File

SOURCE=.\CompositeOverlayDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigureDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ControllerHome.cpp
# End Source File
# Begin Source File

SOURCE=.\ControllerPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\COverlayFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\COverlayHome.cpp
# End Source File
# Begin Source File

SOURCE=.\COverlayPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\CRenameDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CSubject.cpp
# End Source File
# Begin Source File

SOURCE=.\dlldatax.c

!IF  "$(CFG)" == "SysStatsCOM - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "SysStatsCOM - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "SysStatsCOM - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "SysStatsCOM - Win32 Release MinDependency"

!ELSEIF  "$(CFG)" == "SysStatsCOM - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "SysStatsCOM - Win32 Unicode Release MinDependency"

!ELSEIF  "$(CFG)" == "SysStatsCOM - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IINIPersister.cpp
# End Source File
# Begin Source File

SOURCE=.\IniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\IniSection.cpp
# End Source File
# Begin Source File

SOURCE=.\IScriptConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\Label.cpp
# End Source File
# Begin Source File

SOURCE=.\MetaData.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\PluginManager.cpp
# End Source File
# Begin Source File

SOURCE=.\QueueTimerService.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptConnectionObject.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SysStatsCOM.cpp
# End Source File
# Begin Source File

SOURCE=.\SysStatsCOM.def
# End Source File
# Begin Source File

SOURCE=.\SysStatsCOM.idl
# ADD MTL /tlb ".\SysStatsCOM.tlb" /h "SysStatsCOM.h" /iid "SysStatsCOM_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\SysStatsCOM.rc
# End Source File
# Begin Source File

SOURCE=.\WeakReference.cpp
# End Source File
# Begin Source File

SOURCE=.\WeakTarget.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AppConfig.h
# End Source File
# Begin Source File

SOURCE=.\CategoryFactoryT.h
# End Source File
# Begin Source File

SOURCE=.\CClientModel.h
# End Source File
# Begin Source File

SOURCE=.\CCompositeOverlay.h
# End Source File
# Begin Source File

SOURCE=.\CControllerFactory.h
# End Source File
# Begin Source File

SOURCE=.\CMeterDialog.h
# End Source File
# Begin Source File

SOURCE=.\CMeterFactory.h
# End Source File
# Begin Source File

SOURCE=.\CMeterHome.h
# End Source File
# Begin Source File

SOURCE=.\COMINIFile.h
# End Source File
# Begin Source File

SOURCE=.\CompositeOverlayDialog.h
# End Source File
# Begin Source File

SOURCE=.\CompositeOverlayImpl.h
# End Source File
# Begin Source File

SOURCE=.\ConfigureDialog.h
# End Source File
# Begin Source File

SOURCE=.\ControllerHome.h
# End Source File
# Begin Source File

SOURCE=.\ControllerImpl.h
# End Source File
# Begin Source File

SOURCE=.\ControllerPanel.h
# End Source File
# Begin Source File

SOURCE=.\COverlayFactory.h
# End Source File
# Begin Source File

SOURCE=.\COverlayHome.h
# End Source File
# Begin Source File

SOURCE=.\COverlayPanel.h
# End Source File
# Begin Source File

SOURCE=.\CRenameDialog.h
# End Source File
# Begin Source File

SOURCE=.\CSubject.h
# End Source File
# Begin Source File

SOURCE=.\dlldatax.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\help_funcs.h
# End Source File
# Begin Source File

SOURCE=.\IINIPersister.h
# End Source File
# Begin Source File

SOURCE=.\IniFile.h
# End Source File
# Begin Source File

SOURCE=.\IniSection.h
# End Source File
# Begin Source File

SOURCE=.\IScriptConsole.h
# End Source File
# Begin Source File

SOURCE=.\Label.h
# End Source File
# Begin Source File

SOURCE=.\MetaData.h
# End Source File
# Begin Source File

SOURCE=.\MeterImpl.h
# End Source File
# Begin Source File

SOURCE=.\ModelPanel.h
# End Source File
# Begin Source File

SOURCE=.\OverlayImpl.h
# End Source File
# Begin Source File

SOURCE=.\PluginManager.h
# End Source File
# Begin Source File

SOURCE=.\QueueTimerService.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ScriptConnectionObject.h
# End Source File
# Begin Source File

SOURCE=.\ScriptConsole.h
# End Source File
# Begin Source File

SOURCE=.\ScriptUtils.h
# End Source File
# Begin Source File

SOURCE=.\SimpleOverlayImpl.h
# End Source File
# Begin Source File

SOURCE=.\SimpleOverlayImpl2.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TimerClientImpl.h
# End Source File
# Begin Source File

SOURCE=.\WeakReference.h
# End Source File
# Begin Source File

SOURCE=.\WeakTarget.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Animation.rgs
# End Source File
# Begin Source File

SOURCE=.\AppConfig.rgs
# End Source File
# Begin Source File

SOURCE=.\ClientModel.rgs
# End Source File
# Begin Source File

SOURCE=.\COMINIFile.rgs
# End Source File
# Begin Source File

SOURCE=.\CompositeOverlay.rgs
# End Source File
# Begin Source File

SOURCE=.\Controller.rgs
# End Source File
# Begin Source File

SOURCE=.\ControllerHome.rgs
# End Source File
# Begin Source File

SOURCE=.\IINIPersister.rgs
# End Source File
# Begin Source File

SOURCE=.\Label.rgs
# End Source File
# Begin Source File

SOURCE=.\lientModel.rgs
# End Source File
# Begin Source File

SOURCE=.\MetaData.rgs
# End Source File
# Begin Source File

SOURCE=.\Meter.rgs
# End Source File
# Begin Source File

SOURCE=.\meter1.bin
# End Source File
# Begin Source File

SOURCE=.\MeterEvents.rgs
# End Source File
# Begin Source File

SOURCE=.\MeterHome.rgs
# End Source File
# Begin Source File

SOURCE=.\MeterHomeProps.rgs
# End Source File
# Begin Source File

SOURCE=.\Obserrver.rgs
# End Source File
# Begin Source File

SOURCE=.\Overlay.rgs
# End Source File
# Begin Source File

SOURCE=.\OverlayHome.rgs
# End Source File
# Begin Source File

SOURCE=.\Persister.rgs
# End Source File
# Begin Source File

SOURCE=.\ScriptConnectionObject.rgs
# End Source File
# Begin Source File

SOURCE=.\ScriptUtils.rgs
# End Source File
# Begin Source File

SOURCE=.\SimpleOverlay2.rgs
# End Source File
# Begin Source File

SOURCE=.\Tester.rgs
# End Source File
# Begin Source File

SOURCE=.\TimerClient.rgs
# End Source File
# Begin Source File

SOURCE=.\TimerService.rgs
# End Source File
# Begin Source File

SOURCE=.\TreeViewCombo.rgs
# End Source File
# Begin Source File

SOURCE=.\WeakReference.rgs
# End Source File
# Begin Source File

SOURCE=.\WeakTarget.rgs
# End Source File
# Begin Source File

SOURCE=.\Whatever.rgs
# End Source File
# End Group
# Begin Source File

SOURCE=.\SysStatsCOM.manifest
# End Source File
# End Target
# End Project
