# Microsoft Developer Studio Generated NMAKE File, Based on SysStatsApp.dsp
!IF "$(CFG)" == ""
CFG=SysStatsApp - Win32 Debug
!MESSAGE No configuration specified. Defaulting to SysStatsApp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "SysStatsApp - Win32 Release" && "$(CFG)" != "SysStatsApp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SysStatsApp.mak" CFG="SysStatsApp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SysStatsApp - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SysStatsApp - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SysStatsApp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\SysStatsApp.exe"

!ELSE 

ALL : "SysStatsCommon - Win32 Release" "$(OUTDIR)\SysStatsApp.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"SysStatsCommon - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AppConfig.res"
	-@erase "$(INTDIR)\CloseMenuAction.obj"
	-@erase "$(INTDIR)\Main.obj"
	-@erase "$(INTDIR)\Menu.obj"
	-@erase "$(INTDIR)\MenuAction.obj"
	-@erase "$(INTDIR)\ShellIconHelper.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\SysStatsApp.obj"
	-@erase "$(INTDIR)\SysStatsClientWindow.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\WindowDialog.obj"
	-@erase "$(INTDIR)\WindowTimerService.obj"
	-@erase "$(OUTDIR)\SysStatsApp.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\SysStatsCommon" /I "..\SysStatsCOM" /D "NDEBUG" /D _WIN32_WINNT=0x500 /D WINVER=0x500 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\SysStatsApp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AppConfig.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SysStatsApp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\SysStatsApp.pdb" /machine:I386 /out:"$(OUTDIR)\SysStatsApp.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CloseMenuAction.obj" \
	"$(INTDIR)\Main.obj" \
	"$(INTDIR)\Menu.obj" \
	"$(INTDIR)\MenuAction.obj" \
	"$(INTDIR)\SysStatsApp.obj" \
	"$(INTDIR)\SysStatsClientWindow.obj" \
	"$(INTDIR)\WindowDialog.obj" \
	"$(INTDIR)\WindowTimerService.obj" \
	"$(INTDIR)\ShellIconHelper.obj" \
	"$(INTDIR)\AppConfig.res" \
	"$(INTDIR)\StdAfx.obj" \
	"..\SysStatsCommon\Release\SysStatsCommon.lib"

"$(OUTDIR)\SysStatsApp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SysStatsApp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\SysStatsApp.exe" "$(OUTDIR)\SysStatsApp.bsc"

!ELSE 

ALL : "SysStatsCOM - Win32 Debug" "SysStatsCommon - Win32 Debug" "$(OUTDIR)\SysStatsApp.exe" "$(OUTDIR)\SysStatsApp.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"SysStatsCommon - Win32 DebugCLEAN" "SysStatsCOM - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AppConfig.res"
	-@erase "$(INTDIR)\CloseMenuAction.obj"
	-@erase "$(INTDIR)\CloseMenuAction.sbr"
	-@erase "$(INTDIR)\Main.obj"
	-@erase "$(INTDIR)\Main.sbr"
	-@erase "$(INTDIR)\Menu.obj"
	-@erase "$(INTDIR)\Menu.sbr"
	-@erase "$(INTDIR)\MenuAction.obj"
	-@erase "$(INTDIR)\MenuAction.sbr"
	-@erase "$(INTDIR)\ShellIconHelper.obj"
	-@erase "$(INTDIR)\ShellIconHelper.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\SysStatsApp.obj"
	-@erase "$(INTDIR)\SysStatsApp.pch"
	-@erase "$(INTDIR)\SysStatsApp.sbr"
	-@erase "$(INTDIR)\SysStatsClientWindow.obj"
	-@erase "$(INTDIR)\SysStatsClientWindow.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\WindowDialog.obj"
	-@erase "$(INTDIR)\WindowDialog.sbr"
	-@erase "$(INTDIR)\WindowTimerService.obj"
	-@erase "$(INTDIR)\WindowTimerService.sbr"
	-@erase "$(OUTDIR)\SysStatsApp.bsc"
	-@erase "$(OUTDIR)\SysStatsApp.exe"
	-@erase "$(OUTDIR)\SysStatsApp.ilk"
	-@erase "$(OUTDIR)\SysStatsApp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SysStatsCommon" /I "..\SysStatsCOM" /I "..\SysStatsCOM\Debug" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D _WIN32_WINNT=0x500 /D WINVER=0x500 /FR"$(INTDIR)\\" /Fp"$(INTDIR)\SysStatsApp.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AppConfig.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SysStatsApp.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CloseMenuAction.sbr" \
	"$(INTDIR)\Main.sbr" \
	"$(INTDIR)\Menu.sbr" \
	"$(INTDIR)\MenuAction.sbr" \
	"$(INTDIR)\SysStatsApp.sbr" \
	"$(INTDIR)\SysStatsClientWindow.sbr" \
	"$(INTDIR)\WindowDialog.sbr" \
	"$(INTDIR)\WindowTimerService.sbr" \
	"$(INTDIR)\ShellIconHelper.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\SysStatsApp.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  SysStatsCOM.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\SysStatsApp.pdb" /debug /machine:I386 /out:"$(OUTDIR)\SysStatsApp.exe" /pdbtype:sept /libpath:"Debug" /libpath:"..\SysStatsCOM\Debug" 
LINK32_OBJS= \
	"$(INTDIR)\CloseMenuAction.obj" \
	"$(INTDIR)\Main.obj" \
	"$(INTDIR)\Menu.obj" \
	"$(INTDIR)\MenuAction.obj" \
	"$(INTDIR)\SysStatsApp.obj" \
	"$(INTDIR)\SysStatsClientWindow.obj" \
	"$(INTDIR)\WindowDialog.obj" \
	"$(INTDIR)\WindowTimerService.obj" \
	"$(INTDIR)\ShellIconHelper.obj" \
	"$(INTDIR)\AppConfig.res" \
	"$(INTDIR)\StdAfx.obj" \
	"..\SysStatsCommon\Debug\SysStatsCommon.lib" \
	"..\SysStatsCOM\Debug\SysStatsCOM.lib"

"$(OUTDIR)\SysStatsApp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("SysStatsApp.dep")
!INCLUDE "SysStatsApp.dep"
!ELSE 
!MESSAGE Warning: cannot find "SysStatsApp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "SysStatsApp - Win32 Release" || "$(CFG)" == "SysStatsApp - Win32 Debug"
SOURCE=.\AppConfig.rc

"$(INTDIR)\AppConfig.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\CloseMenuAction.cpp

!IF  "$(CFG)" == "SysStatsApp - Win32 Release"


"$(INTDIR)\CloseMenuAction.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "SysStatsApp - Win32 Debug"


"$(INTDIR)\CloseMenuAction.obj"	"$(INTDIR)\CloseMenuAction.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SysStatsApp.pch" ".\ShellIconHelper.cpp"


!ENDIF 

SOURCE=.\Main.cpp

!IF  "$(CFG)" == "SysStatsApp - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\SysStatsCommon" /I "..\SysStatsCOM" /D "NDEBUG" /D _WIN32_WINNT=0x500 /D WINVER=0x500 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\SysStatsApp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "SysStatsApp - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SysStatsCommon" /I "..\SysStatsCOM" /I "..\SysStatsCOM\Debug" /I "Debug" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D _WIN32_WINNT=0x500 /D WINVER=0x500 /FR"$(INTDIR)\\" /Fp"$(INTDIR)\SysStatsApp.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Main.obj"	"$(INTDIR)\Main.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SysStatsApp.pch" ".\ShellIconHelper.cpp"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Menu.cpp

!IF  "$(CFG)" == "SysStatsApp - Win32 Release"


"$(INTDIR)\Menu.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "SysStatsApp - Win32 Debug"


"$(INTDIR)\Menu.obj"	"$(INTDIR)\Menu.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SysStatsApp.pch"


!ENDIF 

SOURCE=.\MenuAction.cpp

!IF  "$(CFG)" == "SysStatsApp - Win32 Release"


"$(INTDIR)\MenuAction.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "SysStatsApp - Win32 Debug"


"$(INTDIR)\MenuAction.obj"	"$(INTDIR)\MenuAction.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SysStatsApp.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "SysStatsApp - Win32 Release"


"$(INTDIR)\StdAfx.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "SysStatsApp - Win32 Debug"


"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SysStatsApp.pch"


!ENDIF 

SOURCE=.\SysStatsApp.cpp

!IF  "$(CFG)" == "SysStatsApp - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\SysStatsCommon" /I "..\SysStatsCOM" /D "NDEBUG" /D _WIN32_WINNT=0x500 /D WINVER=0x500 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\SysStatsApp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\SysStatsApp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "SysStatsApp - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SysStatsCommon" /I "..\SysStatsCOM" /I "..\SysStatsCOM\Debug" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D _WIN32_WINNT=0x500 /D WINVER=0x500 /FR"$(INTDIR)\\" /Fp"$(INTDIR)\SysStatsApp.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\SysStatsApp.obj"	"$(INTDIR)\SysStatsApp.sbr"	"$(INTDIR)\SysStatsApp.pch" : $(SOURCE) "$(INTDIR)" ".\ShellIconHelper.cpp"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\SysStatsClientWindow.cpp

!IF  "$(CFG)" == "SysStatsApp - Win32 Release"


"$(INTDIR)\SysStatsClientWindow.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "SysStatsApp - Win32 Debug"


"$(INTDIR)\SysStatsClientWindow.obj"	"$(INTDIR)\SysStatsClientWindow.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SysStatsApp.pch"


!ENDIF 

SOURCE=.\WindowDialog.cpp

!IF  "$(CFG)" == "SysStatsApp - Win32 Release"


"$(INTDIR)\WindowDialog.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "SysStatsApp - Win32 Debug"


"$(INTDIR)\WindowDialog.obj"	"$(INTDIR)\WindowDialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SysStatsApp.pch"


!ENDIF 

SOURCE=.\WindowTimerService.cpp

!IF  "$(CFG)" == "SysStatsApp - Win32 Release"


"$(INTDIR)\WindowTimerService.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "SysStatsApp - Win32 Debug"


"$(INTDIR)\WindowTimerService.obj"	"$(INTDIR)\WindowTimerService.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SysStatsApp.pch"


!ENDIF 

SOURCE=.\ShellIconHelper.cpp

!IF  "$(CFG)" == "SysStatsApp - Win32 Release"


"$(INTDIR)\ShellIconHelper.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "SysStatsApp - Win32 Debug"


"$(INTDIR)\ShellIconHelper.obj"	"$(INTDIR)\ShellIconHelper.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SysStatsApp.pch"


!ENDIF 

!IF  "$(CFG)" == "SysStatsApp - Win32 Release"

"SysStatsCommon - Win32 Release" : 
   cd "\Program Files\Microsoft Visual Studio\MyProjects\SysStatsCommon"
   $(MAKE) /$(MAKEFLAGS) /F ".\SysStatsCommon.mak" CFG="SysStatsCommon - Win32 Release" 
   cd "..\SysStatsApp"

"SysStatsCommon - Win32 ReleaseCLEAN" : 
   cd "\Program Files\Microsoft Visual Studio\MyProjects\SysStatsCommon"
   $(MAKE) /$(MAKEFLAGS) /F ".\SysStatsCommon.mak" CFG="SysStatsCommon - Win32 Release" RECURSE=1 CLEAN 
   cd "..\SysStatsApp"

!ELSEIF  "$(CFG)" == "SysStatsApp - Win32 Debug"

"SysStatsCommon - Win32 Debug" : 
   cd "\Program Files\Microsoft Visual Studio\MyProjects\SysStatsCommon"
   $(MAKE) /$(MAKEFLAGS) /F ".\SysStatsCommon.mak" CFG="SysStatsCommon - Win32 Debug" 
   cd "..\SysStatsApp"

"SysStatsCommon - Win32 DebugCLEAN" : 
   cd "\Program Files\Microsoft Visual Studio\MyProjects\SysStatsCommon"
   $(MAKE) /$(MAKEFLAGS) /F ".\SysStatsCommon.mak" CFG="SysStatsCommon - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\SysStatsApp"

!ENDIF 

!IF  "$(CFG)" == "SysStatsApp - Win32 Release"

!ELSEIF  "$(CFG)" == "SysStatsApp - Win32 Debug"

"SysStatsCOM - Win32 Debug" : 
   cd "\Program Files\Microsoft Visual Studio\MyProjects\SysStatsCOM"
   $(MAKE) /$(MAKEFLAGS) /F ".\SysStatsCOM.mak" CFG="SysStatsCOM - Win32 Debug" 
   cd "..\SysStatsApp"

"SysStatsCOM - Win32 DebugCLEAN" : 
   cd "\Program Files\Microsoft Visual Studio\MyProjects\SysStatsCOM"
   $(MAKE) /$(MAKEFLAGS) /F ".\SysStatsCOM.mak" CFG="SysStatsCOM - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\SysStatsApp"

!ENDIF 


!ENDIF 

