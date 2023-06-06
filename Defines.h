// Client, HLDS and Client HLDS Definitions
// 
#pragma optimize("gsy", on)
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef Defines_h__
#define Defines_h__
#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOWINRES
#define NOSERVICE
#define NOMCX
#define NOIME
#define NOCOMM
// Set section alignment to be nice and small
#pragma comment(linker, "/FILEALIGN:0x200")

#include <windows.h>
#include "cvardef.h"
#include <stdio.h>
#include "include/detours.h"
#include <malloc.h>
#include <iostream>
#include <fstream>
#include <string>
#include "utils.h"

using namespace std;

#define strtok_r strtok_s

typedef unsigned long DWORD;
typedef unsigned char BYTE;
static bool patch_loaded = 0;
static string download_url				  = "";
static string mapname;

static cvar_t sv_downloadurl				  = { "sv_downloadurl", "", FCVAR_SPONLY};// "Location from which clients can download missing files"};
static cvar_t AllowFragDL				  = { "sv_allow_frag_dl", "1", FCVAR_SPONLY};// "Location from which clients can download missing files"};

int			 SV_ParseCvarValue(int a1);
int			 SV_ParseCvarValue2(int a1);

// standalone hlds
#ifdef sv_hlds

static DWORD base_address				  = (DWORD)GetModuleHandle("swds.dll"); 

static DWORD com_ignorecolons			  = (DWORD)base_address + 0x2DEF28;
static DWORD registervar				  = (DWORD)base_address + 0x26AC0;
static DWORD variablestring				  = (DWORD)base_address + 0x266C0;
static int   gNewDLLFunctions			  = (DWORD)base_address + 0xA9D90;
static DWORD wadpath					  = (DWORD)base_address + 0xE83AC;
static DWORD sv_generic_precache_names	  = (DWORD)base_address + 0x3F79B4;
static DWORD sv_num_generic_names		  = (DWORD)base_address + 0x3EF9B4;

static DWORD svc_hook					  = (DWORD)base_address + 0x168B0;
static DWORD svc_hook_ret				  = (DWORD)base_address + 0x168B6;

static DWORD clc_hook					  =	(DWORD)base_address + 0x948F1;
static DWORD clc_hook_ret				  = (DWORD)base_address + 0x948FB; 

static DWORD masterinit					  = (DWORD)base_address + 0x3AB99;
static DWORD masterinit_ret				  = (DWORD)base_address + 0x3ABA1;

static DWORD nomaster_init_addr			  = (DWORD)base_address + 0x3A866;
static DWORD nomaster_init_ret_addr		  = (DWORD)base_address + 0x3A86B;

static DWORD resourcehook				  = (DWORD)base_address + 0x80A1E;
static DWORD resourcehook_ret			  = (DWORD)base_address + 0x80A24;

static DWORD dlurl_fix					  = (DWORD)base_address + 0x2297B;
static DWORD dlurl_ret					  = (DWORD)base_address + 0x22981;

static DWORD svloadwad					  = (DWORD)base_address + 0x4706b;
static DWORD svloadwad_ret				  = (DWORD)base_address + 0x47070;

static DWORD wad_reslist_fix_addr		  = (DWORD)base_address + 0x9C048;
static DWORD wad_reslist_fix_addr_ret     = (DWORD)base_address + 0x9C04E;

static DWORD tempdecal_fix_addr			  = (DWORD)base_address + 0x9C020;
static DWORD tempdecal_fix_ret			  = (DWORD)base_address + 0x9C038; //match found
static DWORD tempdecal_fix_ret2			  = (DWORD)base_address + 0x9C042; //no pldecal or tempwad found

static DWORD test_addr					  = (DWORD)base_address + 0x86BD1;
static DWORD test_addr_ret				  = (DWORD)base_address + 0x86BD7;

#endif // sv_hlds

// client hlds
#ifdef cl_hlds

static DWORD base_address				  = (DWORD)GetModuleHandle("swds.dll"); 

static DWORD registervar				  = (DWORD)base_address + 0x25F40;
static DWORD variablestring				  = (DWORD)base_address + 0x25B40;
static DWORD com_ignorecolons			  = (DWORD)base_address + 0x0;
static int   gNewDLLFunctions			  = (DWORD)base_address + 0x3A6A50;
static DWORD wadpath					  = (DWORD)base_address + 0x0;

static DWORD svc_hook					  = (DWORD)base_address + 0x0;
static DWORD svc_hook_ret				  = (DWORD)base_address + 0x0;

static DWORD clc_hook					  = (DWORD)base_address + 0x0;
static DWORD clc_hook_ret				  = (DWORD)base_address + 0x0;

static DWORD resourcehook				  = (DWORD)base_address + 0x7F74E;
static DWORD resourcehook_ret			  = (DWORD)base_address + 0x7F754;

static DWORD dlurl_fix					  = (DWORD)base_address + 0x234CB;
static DWORD dlurl_ret					  = (DWORD)base_address + 0x234D1;

static DWORD masterinit					  = (DWORD)base_address + 0x39F73;
static DWORD masterinit_ret				  = (DWORD)base_address + 0x39F79;

static DWORD authinit					  = (DWORD)base_address + 0x95097;
static DWORD authinit_ret				  = (DWORD)base_address + 0x9509E;

static DWORD svloadwad					  = (DWORD)base_address + 0x0;
static DWORD svloadwad_ret				  = (DWORD)base_address + 0x0;

#endif // cl_hlds

#ifdef client

static DWORD base_address				  = (DWORD)GetModuleHandle("hl.exe"); 

void		EngSelect();

int			check_version(void);

void		checkCrc32();

void		CL_SetURL( const char *URL );
const char* CL_GetURL(void);
int __cdecl CL_CheckFile(int a1, int a2);
int			CL_GetTotalDownloadPercent();
int			CL_SetFileDownloadPercent(int);
int			CL_SetTotalDownloadPercent(int);
int			CL_SetFileProgress(int progress);
void		CL_IsThreadFinished();
int			CL_GetFileDownloadPercent();

// The Client states used by the engine
typedef enum
{
	ca_dedicated, 		// A dedicated server with no ability to start a client
	ca_disconnected, 	// Full screen console with no connection
	ca_connecting,      // Challenge requested, waiting for response or to resend connection request.
	ca_connected,		// valid netcon, talking to a server, waiting for server data
	ca_active			// d/l complete, ready game views should be displayed
} cactive_t;

extern bool  engType;

static int   dlvar_count;
static char  dlvararg[4096];
static DWORD com_ignorecolons		  = 0;

static DWORD net_message			  = (DWORD)base_address + 0x2BCCDE4; 
static DWORD registervar		      = (DWORD)base_address + 0x1925CE0;
static DWORD variablestring			  = (DWORD)base_address + 0x1925950;
static int   gNewDLLFunctions		  = (DWORD)base_address + 0x2017120;
static DWORD cls					  = (DWORD)base_address + 0x2BCCDE4;
static DWORD CL_MapName				  = (DWORD)base_address + 0x2D1EE1C;
static DWORD HLVidMode				  = (DWORD)base_address + 0xCF2F8;
static DWORD cmd_addcmd				  = (DWORD)base_address + 0x191E5A0;
static DWORD con_command_addr		  = (DWORD)base_address + 0x191DBB0;
static DWORD net_from				  = (DWORD)base_address + 0x2BD02A4;	// 0x02FD02A4
static DWORD host_initialized		  = (DWORD)base_address + 0x1EE5758;

static DWORD HLDialog_addr			  = (DWORD)base_address + 0x664E0;

static DWORD hostInit_addr			  = (DWORD)base_address + 0x1910D2E;
static DWORD hostInit_ret_addr		  = (DWORD)base_address + 0x1910D38;

static DWORD svc_hook				  = (DWORD)base_address + 0x1913F08;
static DWORD svc_hook_ret			  = (DWORD)base_address + 0x1913F0E;

static DWORD clc_hook				  =	(DWORD)base_address + 0x199C7C6;
static DWORD clc_hook_ret			  =	(DWORD)base_address + 0x199C7CD;

static DWORD resourcehook	    	  = (DWORD)base_address + 0x198A483;
static DWORD resourcehook_ret		  = (DWORD)base_address + 0x198A489;

static DWORD cl_resourcehook		  = (DWORD)base_address + 0x191221B;
static DWORD cl_resourcehook_ret	  = (DWORD)base_address + 0x1912221;
static DWORD cl_resourcehook_stop	  = (DWORD)base_address + 0x19120A1;
static DWORD cl_resourcehook_stop_ret = (DWORD)base_address + 0x19120A9;

static DWORD dlurl_fix				  = (DWORD)base_address + 0x1921107;
static DWORD dlurl_ret				  = (DWORD)base_address + 0x192110D;

static DWORD hostframe				  = (DWORD)base_address + 0x194589C;
static DWORD hostframe_ret			  = (DWORD)base_address + 0x19458A6;

static DWORD host_tempdecal			  = (DWORD)base_address + 0x19A0239;
static DWORD host_tempdecal_ret		  = (DWORD)base_address + 0x19A025C;	// if all good, no pldecal
static DWORD host_tempdecal_ret2	  = (DWORD)base_address + 0x19A024B;	// if pldecal detected

static DWORD host_wadfix			  = (DWORD)base_address + 0x19A03BB;
static DWORD host_wadfix_ret		  = (DWORD)base_address + 0x19A03C2;

typedef int(*HLDialog_orig)(int arg0, char *arg4, ...);
static HLDialog_orig HLDialog		  = reinterpret_cast<HLDialog_orig>(HLDialog_addr);
#endif //Client

#endif // Defines_h__

//////////////////////////////////////////////////////////////////////////
// gets called when console opens:
//////////////////////////////////////////////////////////////////////////
// signed int __usercall sub_40EE40@<eax>(double a1@<st0>, int a2, int a3, int a4)
//////////////////////////////////////////////////////////////////////////
// this is called every time you leave menu and enter menu
//////////////////////////////////////////////////////////////////////////
// BOOL __cdecl sub_463090(int a1, int a2, int a3)	
//////////////////////////////////////////////////////////////////////////
// this gets  called when coming from mp menu to main menu??
//////////////////////////////////////////////////////////////////////////
// HWND __cdecl sub_417BD0(int a1)
// This is called when menu is closed/engine loads
// signed int __usercall sub_416390@<eax>(double a1@<st0>, int a2)
















