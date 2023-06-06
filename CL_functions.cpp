/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
** Date: 2015/04/29 23:22
** File:CL_functions.cpp
** Description: This file contains the NEW functions from Steam HL
**
** Author: Benjamin Shefte
** TODO:
**
** -------------------------------------------------------------------------*/
#include "hooked_funcs.h"

//////////////////////////////////////////////////////////////////////////
//Parses the http download url from svc_resourcelocation
// 
void CL_ParseResourceLocation()
{
	
	int result; // eax@1
	char *v1 = new char; // esi@1
	char *v2 = new char; // eax@4
	result = MSG_ReadString();
	v1 = (char *)result;
	if ( result )
	{
		if ( !_strnicmp("http://", (const char *)result, 7u) || (result = _strnicmp("https://", v1, 8u)) == 0 )
		{ 
			v2 = strrchr(v1, 47);
			if ( !v2 || v2[1] )
			{
				// Current date/time based on current system
		
				&download_url.append(0x80u,(char)result );
				_snprintf((char*)download_url.c_str(), 0x80u, "%s/", v1);
				Con_DPrintf("1. Using %s as primary download location\n", download_url.c_str());
			}
			else
			{
				download_url.append(0x80u,(char)result );
				strncpy((char*)download_url.c_str(), v1, 0x80u);
				Con_DPrintf("2. Using %s as primary download location\n", download_url.c_str());
			}
			CL_SetURL(download_url.c_str());

		}
	}
}

//////////////////////////////////////////////////////////////////////////
//Parses & Sends the requested cvar value to the requester
// 
void CL_Send_CvarValue()

{
	char *v0; // ebx@1
	int v1; // eax@2
	int v2; // ebx@2
	int s; // [sp+0h] [bp-1Ch]@2
	char v5; // [sp+4h] [bp-18h]@2
	
	v0 = (char *)MSG_ReadString();
	if ( strlen(v0) > 0xFE )
	{
		v5 = 10;
		s = (int)cls;
	}
	else
	{
		v1 = Cvar_FindVar(*v0);
		s = (int)cls;
		v2 = v1;
		v5 = 10;
		if ( v1 )
		{
			MSG_WriteByte(s, 10);
			MSG_WriteString((int)&cls, *(DWORD *)(v2 + 4));
			return;
		}
	}
	MSG_WriteByte(s, v5);
	MSG_WriteString((int)&cls, (int)"Bad CVAR request");
	return;
}

//////////////////////////////////////////////////////////////////////////
//Parses & Sends the requested cvar value to the requester - not sure what makes the 2 different
// 
void CL_Send_CvarValue2()
{
	int v0; // esi@1
	int v1; // ebx@1
	int v2; // eax@2
	int v3; // edi@2
	int s; // [sp+0h] [bp-1Ch]@2
	char v6; // [sp+4h] [bp-18h]@2
	
	v0 = MSG_ReadLong();
	v1 = MSG_ReadString();
	if ( strlen((const char *)v1) > 0xFE )
	{
		v6 = 11;
		s = cls;
	}
	else
	{
		v2 = Cvar_FindVar(v1);
		s = cls;
		v3 = v2;
		v6 = 11;
		if ( v2 )
		{
			MSG_WriteByte(s, 11);
			MSG_WriteLong(cls, v0);
			MSG_WriteString(cls, v1);
			MSG_WriteString(cls, *(DWORD *)(v3 + 4));
			return;
		}
	}
	MSG_WriteByte(s, v6);
	MSG_WriteLong(cls, v0);
	MSG_WriteString(cls, v1);
	MSG_WriteString(cls, (int)"Bad CVAR request");
	return;
}

#if 0
//Exposed to mods through newdllfunctions

void *__cdecl QueryClientCvarValue(char *a1, int a2)
{
	int v2; // eax@1
	void (__cdecl *v3)(_DWORD, _DWORD); // eax@3
	void *result; // eax@5
	int v5; // ebx@6
	
	v2 = NUM_FOR_EDICT(a1);
	if ( v2 > 0 && v2 <= *(&svs + 2) )
	{
		v5 = *(&svs + 1) + 20212 * (v2 - 1) + 116;
		MSG_WriteChar(v5, 57);
		result = MSG_WriteString(v5, a2);
	}
	else
	{
		v3 = *(&gNewDLLFunctions + 3);
		if ( v3 )
			v3(a1, "Bad Player");
		result = Con_Printf("tried to QueryClientCvarValue a non-client\n", a2);
	}
	return result;
}

void *__cdecl QueryClientCvarValue2(char *a1, int a2, int a3)
{
	int v3; // eax@1
	void (__cdecl *v4)(DWORD, DWORD, DWORD, DWORD); // eax@3
	void *result; // eax@5
	int v6; // ebx@6
	
	v3 = NUM_FOR_EDICT(a1);
	if ( v3 > 0 && v3 <= *(&svs + 2) )
	{
		v6 = *(&svs + 1) + 20212 * (v3 - 1) + 116;
		MSG_WriteChar(v6, 58);
		MSG_WriteLong(v6, a3);
		result = MSG_WriteString(v6, a2);
	}
	else
	{
		v4 = *(&gNewDLLFunctions + 4);
		if ( v4 )
			v4(a1, a3, a2, "Bad Player");
		result = Con_Printf("tried to QueryClientCvarValue a non-client\n", a2);
	}
	return result;
}
#endif