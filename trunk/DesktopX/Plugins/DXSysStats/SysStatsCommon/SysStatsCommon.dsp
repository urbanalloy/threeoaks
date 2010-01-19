# Microsoft Developer Studio Project File - Name="SysStatsCommon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SysStatsCommon - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SysStatsCommon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SysStatsCommon.mak" CFG="SysStatsCommon - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SysStatsCommon - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SysStatsCommon - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SysStatsCommon - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SysStatsCommon - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_ATL_DLL" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "SysStatsCommon - Win32 Release"
# Name "SysStatsCommon - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ActiveScriptImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\APVDummy.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapHTTPSink.cpp
# End Source File
# Begin Source File

SOURCE=.\Config.rc
# End Source File
# Begin Source File

SOURCE=.\Dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DictionaryPtr.cpp
# End Source File
# Begin Source File

SOURCE=.\HDC.cpp
# End Source File
# Begin Source File

SOURCE=.\HDCImage.cpp
# End Source File
# Begin Source File

SOURCE=.\Menu.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuAction.cpp
# End Source File
# Begin Source File

SOURCE=.\RemoteBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\SysStatsControllerCategory.cpp
# End Source File
# Begin Source File

SOURCE=.\SysStatsMeterCategory.cpp
# End Source File
# Begin Source File

SOURCE=.\SysStatsOverlayCategory.cpp
# End Source File
# Begin Source File

SOURCE=.\SysStatsUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeViewCombo.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ActiveScriptImpl.h
# End Source File
# Begin Source File

SOURCE=.\APVDummy.h
# End Source File
# Begin Source File

SOURCE=.\ArrayPtr.h
# End Source File
# Begin Source File

SOURCE=.\ArrayPtrVariant.h
# End Source File
# Begin Source File

SOURCE=.\BitmapHTTPSink.h
# End Source File
# Begin Source File

SOURCE=.\CommonDialog.h
# End Source File
# Begin Source File

SOURCE=.\Dialog.h
# End Source File
# Begin Source File

SOURCE=.\DictionaryPtr.h
# End Source File
# Begin Source File

SOURCE=.\HDC.h
# End Source File
# Begin Source File

SOURCE=.\HDCImage.h
# End Source File
# Begin Source File

SOURCE=.\Menu.h
# End Source File
# Begin Source File

SOURCE=.\MenuAction.h
# End Source File
# Begin Source File

SOURCE=.\MenuActionT.h
# End Source File
# Begin Source File

SOURCE=.\PickList.h
# End Source File
# Begin Source File

SOURCE=.\RemoteBitmap.h
# End Source File
# Begin Source File

SOURCE=.\SysStatsControllerCategory.h
# End Source File
# Begin Source File

SOURCE=.\SysStatsMeterCategory.h
# End Source File
# Begin Source File

SOURCE=.\SysStatsOverlayCategory.h
# End Source File
# Begin Source File

SOURCE=.\SysStatsUtils.h
# End Source File
# Begin Source File

SOURCE=.\TreeViewCombo.h
# End Source File
# End Group
# End Target
# End Project
