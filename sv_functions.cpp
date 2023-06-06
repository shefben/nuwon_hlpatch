/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
** Date: 2015/04/29 23:28
** File:sv_functions.cpp
** Description: Contains the New functions from steam HL that belong to the
** Server ONLY
**
** Author: Benjamin Shefte
** TODO:
**
** -------------------------------------------------------------------------*/

#include "Defines.h"
#include "cvardef.h"
#include "hooked_funcs.h"
//////////////////////////////////////////////////////////////////////////
//These parse clc_cvarvalue1 & 2
// 
#ifndef client
static 
#endif
int SV_ParseCvarValue(int a1)
{
	int v1; // esi@1
	void (__cdecl *v2)(DWORD, DWORD); // eax@1
	
	v1 = MSG_ReadString();
	v2 = (void (__cdecl *)(DWORD, DWORD))*(&gNewDLLFunctions + 3);
	if ( v2 )
		v2(*(DWORD *)(a1 + 19076), v1);
	return Con_Printf("Cvar query response: name:%s, value:%s\n", a1 - 24);
}
#ifndef client
static 
#endif
int SV_ParseCvarValue2(int a1)
{
	int v1; // edi@1
	const char *v2; // eax@1
	int v3; // ebp@1
	void (__cdecl *v4)(DWORD, DWORD, DWORD, DWORD); // eax@1
	char v6; // [sp+21h] [bp-11Bh]@1
	char v7; // [sp+11Fh] [bp-1Dh]@1
	
	v1 = MSG_ReadLong();
	v2 = (const char *)MSG_ReadString();
	strncpy(&v6, v2, 0xFFu);
	v7 = 0;
	v3 = MSG_ReadString();
	v4 = (void (__cdecl *)(DWORD, DWORD, DWORD, DWORD))*(&gNewDLLFunctions + 4);
	if ( v4 )
		v4(*(DWORD *)(a1 + 19076), v1, (unsigned long)&v6, v3);
	return Con_Printf("Cvar query response: name:%s, request ID %d, cvar:%s, value:%s\n", a1 - 24);
}
