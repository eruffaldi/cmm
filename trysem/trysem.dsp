# Microsoft Developer Studio Project File - Name="trysem" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=trysem - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "trysem.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "trysem.mak" CFG="trysem - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "trysem - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "trysem - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "trysem - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "trysem - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "trysem - Win32 Release"
# Name "trysem - Win32 Debug"
# Begin Source File

SOURCE=..\axpgen.cpp
# End Source File
# Begin Source File

SOURCE=..\axpgen.h
# End Source File
# Begin Source File

SOURCE=..\bools.h
# End Source File
# Begin Source File

SOURCE=..\errors.h
# End Source File
# Begin Source File

SOURCE=..\gencode.cpp
# End Source File
# Begin Source File

SOURCE=..\gencode.h
# End Source File
# Begin Source File

SOURCE=..\htable.h
# End Source File
# Begin Source File

SOURCE=..\keywords.h
# End Source File
# Begin Source File

SOURCE=..\NatSet.h
# End Source File
# Begin Source File

SOURCE=..\node.cpp
# End Source File
# Begin Source File

SOURCE=..\node.h
# End Source File
# Begin Source File

SOURCE=..\nparser.cpp
# End Source File
# Begin Source File

SOURCE=..\pair.h
# End Source File
# Begin Source File

SOURCE=..\parse_expr.cpp
# End Source File
# Begin Source File

SOURCE=..\parsedecl.cpp
# End Source File
# Begin Source File

SOURCE=..\parser.h
# End Source File
# Begin Source File

SOURCE=..\regalloc.h
# End Source File
# Begin Source File

SOURCE=..\Scan.cpp
# End Source File
# Begin Source File

SOURCE=..\Scan.h
# End Source File
# Begin Source File

SOURCE=..\semantic.cpp
# End Source File
# Begin Source File

SOURCE=..\semantic.h
# End Source File
# Begin Source File

SOURCE=..\slista.h
# End Source File
# Begin Source File

SOURCE=..\symbol.cpp
# End Source File
# Begin Source File

SOURCE=..\symbol.h
# End Source File
# Begin Source File

SOURCE=..\trysem.cpp
# End Source File
# Begin Source File

SOURCE=..\types.cpp
# End Source File
# Begin Source File

SOURCE=..\types.h
# End Source File
# End Target
# End Project
