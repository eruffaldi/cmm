# Microsoft Developer Studio Project File - Name="cmm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=cmm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cmm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cmm.mak" CFG="cmm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cmm - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "cmm - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cmm - Win32 Release"

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

!ELSEIF  "$(CFG)" == "cmm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "cmm___Wi"
# PROP BASE Intermediate_Dir "cmm___Wi"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
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

# Name "cmm - Win32 Release"
# Name "cmm - Win32 Debug"
# Begin Source File

SOURCE=.\src\autoptr.h
# End Source File
# Begin Source File

SOURCE=.\src\Axpgen.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Axpgen.h
# End Source File
# Begin Source File

SOURCE=.\src\bools.h
# End Source File
# Begin Source File

SOURCE=.\src\btree.h
# End Source File
# Begin Source File

SOURCE=.\src\cmm.cpp
# End Source File
# Begin Source File

SOURCE=.\src\cmm.h
# End Source File
# Begin Source File

SOURCE=.\src\dvector.h
# End Source File
# Begin Source File

SOURCE=.\src\errors.h
# End Source File
# Begin Source File

SOURCE=.\src\errorTable.h
# End Source File
# Begin Source File

SOURCE=.\src\Format.h
# End Source File
# Begin Source File

SOURCE=.\src\Format_impl.h
# End Source File
# Begin Source File

SOURCE=.\src\gencode.cpp
# End Source File
# Begin Source File

SOURCE=.\src\gencode.h
# End Source File
# Begin Source File

SOURCE=.\src\htable.h
# End Source File
# Begin Source File

SOURCE=.\src\htable_impl.h
# End Source File
# Begin Source File

SOURCE=.\src\keywords.h
# End Source File
# Begin Source File

SOURCE=.\src\main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\mappa.h
# End Source File
# Begin Source File

SOURCE=.\src\NatSet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\NatSet.h
# End Source File
# Begin Source File

SOURCE=.\src\Node.cpp
# End Source File
# Begin Source File

SOURCE=.\src\node.h
# End Source File
# Begin Source File

SOURCE=.\src\nparser.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pair.h
# End Source File
# Begin Source File

SOURCE=.\src\parse_expr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\parsedecl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\parser.h
# End Source File
# Begin Source File

SOURCE=.\src\preproc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\preproc.h
# End Source File
# Begin Source File

SOURCE=.\src\Scan.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Scan.h
# End Source File
# Begin Source File

SOURCE=.\src\Semantic.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Semantic.h
# End Source File
# Begin Source File

SOURCE=.\src\slista.h
# End Source File
# Begin Source File

SOURCE=.\doc\Structures.txt
# End Source File
# Begin Source File

SOURCE=.\src\symbol.cpp
# End Source File
# Begin Source File

SOURCE=.\src\symbol.h
# End Source File
# Begin Source File

SOURCE=.\todo.txt
# End Source File
# Begin Source File

SOURCE=.\src\token.cpp
# End Source File
# Begin Source File

SOURCE=.\src\token.h
# End Source File
# Begin Source File

SOURCE=.\src\types.cpp
# End Source File
# Begin Source File

SOURCE=.\src\types.h
# End Source File
# Begin Source File

SOURCE=.\src\visitor.h
# End Source File
# Begin Source File

SOURCE=.\src\xmlout.cpp
# End Source File
# End Target
# End Project
