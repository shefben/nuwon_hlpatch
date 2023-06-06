//Tis function is used to parse the customized files.
//The server wiill send the sv_downloadurl cvar to client first
// Then hook will disconnect the client, download the files & reconnect.
// Net_message Address: 02A170E0
// Net_message_ptr Address: 02A170E8
//[char] Net_message_buffer / (sizebuf_t)msg->&data address: 0x2A17120 
//number of resources first 12 bits
//resource type, next 4 bits
// name is unknown string, length is probably first bit
// indext is next 12 bits
// download size is next 24 bits
// flags are the next 3bits probably xored
// 
// 
//Half-life v46 is currently missing only 3 messages: svc_resourcelocation, svc_send_cvarvalue & svc_send_cvarvalue 2
// clc_cvarvalue & clc_cvarvalue2

//NOTE: Must not hook until in console/connecting to server.  or else it does not detour
#include "Defines.h"
#include <detours.h>
// #include <malloc.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "clientmessages.h"
#include "servermessages.h"
#include "utils.h"
#include "hooked_funcs.h"
#include "hooked_client.h"
#include "download.h"
#include "download_internal.h"


using namespace std;

typedef int(*Orig_CL_CheckFile)( int, int*);
Orig_CL_CheckFile oHook; // our original function

typedef int(*Orig_IsSafeFileToDownload)(char*);
Orig_IsSafeFileToDownload filesafe_hook;

typedef int(*Orig_SCR_DrawDownloadText_1)( int, int);
Orig_SCR_DrawDownloadText_1 drawdlHook; // our original function
bool engType;
//replaces half-lifes' cl parse command struct with our own 
//Adds svc_resourcelocation, svc_sendcvar/2
/*
.text:01D13F08                 mov     edi, cl_parsefuncs[ecx]
.text:01D13F0E ; 77:       if ( v10 ) 

  text:01D13ABD                 mov     edi, svc_messages[ecx]
.text:01D13AC3 ; 77:       if ( v11 )
*/
__declspec(naked) void svc_message_hook() 
{	
	__asm 
	{ 
		mov edi, cl_parsefuncs[ ecx ].pfnParse
		jmp [svc_hook_ret]
	}	
}
//replaces half-lifes' server parse command struct with our own
//adds clc_querycvar/2
/*
.text:01D9C7C6                 mov     eax, dword_1E05D40[eax*4]
.text:01D9C7CD ; 47:     if ( v9 )
*/
int clcmsg;
__declspec(naked) void clc_message_hook() 
{	
	__asm
	{ // pushad
		mov clcmsg, eax
		mov eax, sv_clcfuncs[eax*4].pfnParse
		jmp [clc_hook_ret]
	}	
}

//SV_sendresourcelist - added sv_downloadurl to this packet (same way as steam)
//0x01D809F0
/*
client HardWare:
.text:01D8A483                 mov     ebp, [esp+34h+arg_0]
.text:01D8A487                 push    2Dh
.text:01D8A489                 push    ebp

*/
int a1;
__declspec(naked) void sendResource_Hook() 
{	
	
	__asm
	{    
		mov ebp, [esp+56]
		mov a1, ebp
	}	
	if(downloadurl.string[0])
	{
		MSG_WriteByte(a1, 56);
		MSG_WriteString(a1, reinterpret_cast<int>(downloadurl.string));
	}
	__asm
	{
		push 2Dh
		jmp [resourcehook_ret]
	}
	
}
//Should go with disconnect, right?
// .text:01D120A1                 mov     dword ptr [esp+24], 10000h
// .text:01D120A9 ; 54:   if ( dword_2FD3A6C )
__declspec(naked) void HTTPStop_Hook() 
{	
	__asm 
	{ 
		mov dword ptr [esp+0x24], 10000h
		call CL_HTTPStop_f
		jmp [cl_resourcehook_stop_ret]
	}	
}

//This function is to add the http download function/stuff to the clients resource list parser
//CL_StartResourceDownloading
/*
.text:01D1221B                 mov     eax, cls_0_
.text:01D12220                 pop     edi
.text:01D12221                 pop     esi
*/
int totalpercent;
 __declspec(naked) void cl_downloadres_hook() 
{	
	__asm
	{  
			pushad
	}	

	 if ( CL_GetDownloadQueueSize() > 0 )
	 {
		 CL_MarkMapAsUsingHTTPDownload();
		 CL_DownloadUpdate();
		 //cls_0 = 1;
		// CL_Disconnect();
	 }

  __asm{
		popad
	    mov eax, cls_0
		pop edi
		jmp [cl_resourcehook_ret]
	}
}
/* 
.text:01D45897 E8 74 FE FF FF                                call    Host_CheckConnectionFailure ; Call Procedure
.text:01D45897
.text:01D4589C                               ; 47:         ClientDLL_CAM_Think();
.text:01D4589C E8 0F D0 FB FF                                call    ClientDLL_CAM_Think ; Call Procedure
.text:01D4589C
.text:01D458A1                               ; 48:         CL_MoveSpectatorCamera();
.text:01D458A1 E8 8A 15 FD FF                                call    CL_MoveSpectatorCamera ; Call Procedure
.text:01D458A1
*/
__declspec(naked) void Host_frame_fix()
 {
/*	__asm
	{
		pushad
	}*/
		CL_DownloadUpdate();
	

	__asm
	{
		//popad
		call Host_CheckConnectionFailure
		call ClientDLL_CAM_Think
		jmp [hostframe_ret]
	}
 }

/*
.text:01D21107 8B 15 B8 71 F9 01                             mov     edx, com_ignorecolons
.text:01D2110D                               ; 68:         if ( (com_ignorecolons || v3 != ':') && v3 != ',' )
.text:01D2110D 3B D3                                         cmp     edx, ebx        ; Compare Two Operands .*/
// int command;
 __declspec(naked) void dlurl_token() 
 {	
	 __asm
	 {
		 cmp  edx, ebx
		 jmp [dlurl_ret]
	 }
}

//////////////////////////////////////////////////////////////////////////
//Check each file for their existence
// 
int __cdecl CL_CheckFile(int a1, int a2)
{
	string test;
	test = (const char*)a2;
	if (test.find(".wad"))
	{
		MessageBox(NULL, "wad", (const char*)a2, MB_OK);
	}
	if ( strstr((const char*)a2, "..") || strstr((const char*)a2, "server.cfg") )
	{
		Con_DPrintf("Refusing to download a path with '..'\n");
		return 1;
	}
	if ( cl_allow_download == 0.0 )
	{
		Con_DPrintf("Download refused, cl_allow_download is 0\n");
		return 1;
	}
	if ( cls_0 == 5 && cl_download_ingame == 0.0 )
	{
		Con_DPrintf("In-game download refused...\n");
		return 1;
	}
	if ( FS_FileExists(a2) != -1 ) //  -1 = missing 
		return 1;
	if ( unknown_1 == 1 || unknown_2 ==1 )//probably just used to check if we are in a demo
	{
		Con_Printf("Warning! File %s missing during demo playback.\n", a2);
		return 1;
	}
	else if (CL_CanUseHTTPDownload() != 0)
 	{
		CL_QueueDownload((const char*)a2);
		*(BYTE*)gDownloadFile = (BYTE)((const char*)a2);

		return 1;
	}
	else
	{
		MSG_WriteByte(a1, 3);
		MSG_WriteString(a1, (int)va("dlfile %s", a2));
		return 0;
	}	
	
	return 0;
}


/*
.text:01D3A479                 call    GLFinishHud     ; Call Procedure
.text:01D3A479
.text:01D3A47E ; 56:         if ( off_22E6218 )
*/
 DWORD scr_updatescreen_return = (DWORD)base_address + 0x193A47E;
__declspec(naked) void SCR_UpdateScreen_Hook() 
{	
	__asm
	{
			jmp [scr_updatescreen_return]
	}
}

/*
.text:01D1221B                 mov     eax, dword_2FCCD80
.text:01D12220 ; 129:   if ( dword_2FCCD80 != 1 )
.text:01D12220                 pop     edi
*/
__declspec(naked) void ResourcePrecache_Hook() 
{	
//	while( CL_GetDownloadQueueSize() != 0)
	//	;
	__asm
	{
		mov eax, dword_2FCCD80
		pop edi

		jmp [ResourcePrecache_return]
	}
}

void SCR_DrawDownloadText_1()
{
	signed int v0; // eax@7
	int total_progress = CL_GetTotalDownloadPercent();
	int file_progress = CL_GetFileDownloadPercent();
	int downloadingResources = -1;
//	*(BYTE*)scr_downloading = 0;
	if (sizeof(CL_GetURL()) <= 0)
	{
		if ( *(BYTE*)scr_downloading >= 0.0 && !byte_1E120A8  )
		{
			if ( (sub_1D51A30((int)&dword_2FCCD88) || sub_1D51A60((int)&dword_2FCCD88)) && !dword_2FD0EF8 && dword_2FCCD80 != 1 )
			{
				v0 = *(BYTE*)scr_downloading < 0 ? 0 : *(BYTE*)scr_downloading;
				if ( v0 > 100 )
					v0 = 100;
				SCR_DrawDownloadText(v0, 1);
			}
			else
			{
				*(BYTE*)scr_downloading = 0.0;
			}
		}
	}
	else if (CL_DownloadUpdate() != NULL)
	{
		v0 = *(BYTE*)scr_downloading < 0 ? 0 : *(BYTE*)scr_downloading;
		if ( v0 > 100 ){
			*(BYTE*)gDownloadFile = (BYTE)"";
			v0 = 100;
			//SCR_DrawDownloadText(v0, 0);
		}
		SCR_DrawDownloadText(v0, 1);
	//	CL_IsThreadFinished();
// 		if (m_totalRequests == m_failedRequests + m_filesCompleted)
// 		{
// 			Con_Command("retry\n");
// 		}


	}
}


//////////////////////////////////////////////////////////////////////////
//Our entry point
// TO-DO:
// Cvar_Set((int)"c// 	else
// 	{
// 		if(progress > 0){ 		 
// 			SCR_DrawDownloadText(progress, 1);
// 		}
// 		else{
// 			*(BYTE*)scr_downloading = 0.0;
// 			//SCR_DrawDownloadText(0,0);
// 			//SCR_DrawDownloadText(0,1);
// 			}
// 	}
//cl_dlmax", 1024);
// add cl_weaponlistfix cvar (ignores weapon list)for cs1.6/cz

void callPatches()
{
	Con_Printf("\nNeuwon Module Loaded Successfully\n");
	//Client Only hooks
	
	DWORD Hookaddress;
	DWORD drawdlHook_address;
//	DWORD resprecache_address;
	DWORD filesafe_hook_address;
	if(engType == 1)
	{
		Hookaddress = ((DWORD)0x1911520 + (DWORD)base_address); //0x01D11520
		drawdlHook_address = ((DWORD)0x193AAC0 + (DWORD)base_address);//0x01D3AAC0

		filesafe_hook_address = 0x1911670 + (DWORD)base_address; //0x01D11670
		//httpstop_address = 0x19120A1 + (DWORD)base_address; //0x01D120A1
	//	resprecache_address = 0x1912220 + (DWORD)base_address;//0x01D12220
	}
	else //software
	{
		Hookaddress = ((DWORD)0x19111B0 + (DWORD)base_address); // 0x01D111B0
		drawdlHook_address = ((DWORD)0x196DE80 + (DWORD)base_address); // 0x01D6DE800x01D11520
	//	filesafe_hook_address = 0x19112F0 + (DWORD)base_address; // 0x01D112F0
	}
	(Orig_CL_CheckFile) DetourFunction((PBYTE)Hookaddress, (PBYTE) CL_CheckFile);
	(Orig_SCR_DrawDownloadText_1) DetourFunction((PBYTE) drawdlHook_address, (PBYTE) SCR_DrawDownloadText_1);
	(Orig_IsSafeFileToDownload) DetourFunction((PBYTE)filesafe_hook_address, (PBYTE) IsSafeFileToDownload);

		placeJMP((BYTE*)cl_resourcehook, (DWORD)cl_downloadres_hook, 6); //send download url if available
		//placeJMP((BYTE*)(DWORD)base_address + 0x1912220, (DWORD)ResourcePrecache_Hook, 6); //fix to allow semi colon for dlurl
	//	placeJMP((BYTE*)hostframe, (DWORD)Host_frame_fix, 10); //fix to allow semi colon for dlurl
	//	placeJMP((BYTE*)dlurl_fix, (DWORD)dlurl_token, 6); //fix to allow semi colon for dlurl
		//placeJMP((BYTE*)(DWORD)base_address + 0x19120A1, (DWORD)HTTPStop_Hook, 8); //fix to allow semi colon for dlurl

		//Shared (Server and Client) Hooks

		//Cmd_AddCommand("httpstop", ((int)void(*))CL_HTTPStop_f());
		//Cvar_Set((int)"cl_dlmax", 1024);
		Cvar_RegisterVariable(reinterpret_cast<int*>(&AllowFragDL));
		Cvar_RegisterVariable(reinterpret_cast<int*>(&downloadurl));
		placeJMP((BYTE*)clc_hook, (DWORD)clc_message_hook, 7); //new clc messages
		placeJMP((BYTE*)svc_hook, (DWORD)svc_message_hook, 6); //new svc messages
		placeJMP((BYTE*)resourcehook, (DWORD)sendResource_Hook, 6); //send download url if available


}
extern "C"
{	
	void __cdecl export(void)
	{
		// THIS IS JUST A PLACE HOLDER SO THE DLL LOADS WITH HL
	}
}
__declspec(naked) void dllLoadLoop()
{
	__asm
	{ 
		pop esi
			add     esp, 0Ch
			pushad
	}
	//	engType = 0;
		EngSelect(); //Determin which engine is in use
	callPatches();
	__asm
	{
		popad
			retn
	}
	
	
}

DWORD dword_3CE773C =	base_address + 0x38E773C;
DWORD dword_3CE7738 =	base_address + 0x38E7738;
void EngSelect()
{
	if(*(BYTE*)dword_3CE773C || *(BYTE*)dword_3CE7738) // 0 or 0 == software, 0 or 1 == opengl, 1 or 0 == d3d
		engType = 1;
	else
		engType = 0;
}
//////////////////////////////////////////////////////////////////////////
//Our entry point
//cl_dlmax", 1024);

HLDialog_orig HLDialog;



void hookMOTD()
{
	
	//
	//	HLDialog = reinterpret_cast<HLDialog_orig>((DWORD)0x664E0+(DWORD)base_address);
	//	HLDialog(NULL, "This is a test", "test this");
	
}

typedef DWORD(*wnd_createpopupdlg)(DWORD a1, int a2, int a3);
wnd_createpopupdlg update_wnd;
DWORD update_wndAddr = ((DWORD)GetModuleHandle( "hl.exe" ) + (DWORD)0xC220); //0040C220;
DWORD update_return = ((DWORD)GetModuleHandle( "hl.exe" ) + (DWORD)0x2773D); //0042773D
DWORD loginwnd;
__declspec(naked) void gui_updater()
{
	__asm
	{
		
		pushad
			mov loginwnd, ecx
			
	}
	if(check_version() == 0){
		HLDialog = reinterpret_cast<HLDialog_orig>((DWORD)0x664E0+(DWORD)base_address);
		HLDialog(loginwnd, "There is a NEW UPDATE!\n Please download the patch to play online!");
		PostQuitMessage(0);
		WinExec("sierraup.exe", 5u);
	}
	__asm
	{
		popad
			mov     eax, [esp+20h]
			push    ebx
			jmp[update_return]
	}
}
BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved )
{
	
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//HL.EXE hook
		(DWORD)base_address= (DWORD)GetModuleHandle("hl.exe"); 
		//ReRoute(MainModule, pNTHeader,&gppt,         "Kernel32.dll","GetProcAddress",&gpoffset);
		//checkCrc32();
		placeJMP((BYTE*)((DWORD)GetModuleHandle("hl.exe") + (DWORD)0x164C0), (DWORD)dllLoadLoop, 5);
		//placeJMP((BYTE*)base_address + (DWORD)0x27738, (DWORD)gui_updater, 5); //00427738
		
		return true;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}