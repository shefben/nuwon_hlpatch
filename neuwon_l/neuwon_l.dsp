# Microsoft Developer Studio Project File - Name="neuwon_l" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=neuwon_l - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "neuwon_l.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "neuwon_l.mak" CFG="neuwon_l - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "neuwon_l - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "neuwon_l - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "neuwon_l - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f neuwon_l.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "neuwon_l.exe"
# PROP BASE Bsc_Name "neuwon_l.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake /f "neuwon_l.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "neuwon_l.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "neuwon_l - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f neuwon_l.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "neuwon_l.exe"
# PROP BASE Bsc_Name "neuwon_l.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "neuwon_l.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "neuwon_l.so"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "neuwon_l - Win32 Release"
# Name "neuwon_l - Win32 Debug"

!IF  "$(CFG)" == "neuwon_l - Win32 Release"

!ELSEIF  "$(CFG)" == "neuwon_l - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;asm"
# Begin Source File

SOURCE=.\boffix.c
# End Source File
# Begin Source File

SOURCE=.\hook_utils.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\utilities.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc"
# Begin Source File

SOURCE=.\hook_utils.h
# End Source File
# Begin Source File

SOURCE=.\main_structs.h
# End Source File
# Begin Source File

SOURCE=.\neuwon.h
# End Source File
# Begin Source File

SOURCE=.\sdkinclude.h
# End Source File
# Begin Source File

SOURCE=.\utilities.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
