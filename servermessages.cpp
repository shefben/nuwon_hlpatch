#include "hooked_funcs.h"
// #include "utils.h"
//contains all the functions we need for the new msg types
// 
int __cdecl SV_ParseCvarValue(int a1)
{
	int v1; // esi@1
	void (__cdecl *v2)(DWORD, DWORD); // eax@1
	
	v1 = MSG_ReadString();
	v2 = (void (__cdecl *)(DWORD, DWORD))*(&gNewDLLFunctions + 3);
	if ( v2 )
		v2(*(DWORD *)(a1 + 19076), v1);
return	Con_Printf("Cvar query response: name:%s, value:%s\n", a1 - 24);
}

int __cdecl SV_ParseCvarValue2(int a1)
{
	int v1; // edi@1
	const char *v2; // eax@1
	int v3; // ebp@1
	void (__cdecl *v4)(DWORD, DWORD, DWORD, DWORD); // eax@1
	char v6; // [sp+21h] [bp-11Bh]@1
	char v7; // [sp+11Fh] [bp-1Dh]@1
	
	v1 = MSG_ReadLong();
	v2 = (const char *)MSG_ReadString();
	Q_strncpy(&v6, v2, 0xFFu);
	v7 = 0;
	v3 = MSG_ReadString();
	v4 = (void (__cdecl *)(DWORD, DWORD, DWORD, DWORD))*(&gNewDLLFunctions + 4);
	if( v4 )
		v4(*(DWORD *)(a1 + 19076), v1, (unsigned long)&v6, v3);
	return Con_Printf("Cvar query response: name:%s, request ID %d, cvar:%s, value:%s\n", a1 - 24);
}

#if 0
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
		v3 = (void (__cdecl *)(_DWORD, _DWORD))*(&gNewDLLFunctions + 3);
		if ( v3 )
			v3(a1, "Bad Player");
		result = (void *)Con_Printf("tried to QueryClientCvarValue a non-client\n", a2);
	}
	return result;
}

void *__cdecl QueryClientCvarValue2(char *a1, int a2, int a3)
{
	int v3; // eax@1
	void (__cdecl *v4)(_DWORD, _DWORD, _DWORD, _DWORD); // eax@3
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
		v4 = (void (__cdecl *)(_DWORD, _DWORD, _DWORD, _DWORD))*(&gNewDLLFunctions + 4);
		if ( v4 )
			v4(a1, a3, a2, "Bad Player");
		result = (void *)Con_Printf("tried to QueryClientCvarValue a non-client\n", a2);
	}
	return result;
}
int QueryClientCvarValueCmd()
{
	int result; // eax@2
	int v1; // ebx@2
	int v2; // esi@3
	char *v3; // eax@6
	char *v4; // eax@11
	int v5; // ebx@11
	int v6; // esi@11
	int v7; // eax@11
	void (__cdecl *v8)(_DWORD); // eax@13
	char v9; // al@16
	int v10; // ebx@17
	char *v11; // [sp+4h] [bp-18h]@6
	
	if ( Cmd_Argc() <= 1 )
	{
		v9 = (unsigned int)Cmd_Argv(0);
		return Con_Printf("%s <player name> <cvar>\n", v9);
	}
	result = *(&svs + 2);
	v1 = *(&svs + 1);
	if ( result > 0 )
	{
		v2 = 0;
		do
		{
			if ( *(_DWORD *)v1 || *(_DWORD *)(v1 + 12) )
			{
				v3 = Cmd_Argv(1u);
				result = Q_strcasecmp((char *)(v1 + 19432), v3);
				if ( !result )
				{
					if ( *(&svs + 2) > v2 )
					{
						v4 = Cmd_Argv(2u);
						v5 = *(_DWORD *)(v1 + 19076);
						v6 = (int)v4;
						v7 = NUM_FOR_EDICT(v5);
						if ( v7 > 0 && v7 <= *(&svs + 2) )
						{
							v10 = *(&svs + 1) + 20212 * (v7 - 1) + 116;
							MSG_WriteChar(v10, 57);
							result = (int)MSG_WriteString(v10, v6);
						}
						else
						{
							v8 = (void (__cdecl *)(_DWORD))*(&gNewDLLFunctions + 3);
							if ( v8 )
							{
								v11 = "Bad Player";
								v8(v5);
							}
							result = Con_Printf("tried to QueryClientCvarValue a non-client\n", (char)v11);
						}
					}
					return result;
				}
				result = *(&svs + 2);
			}
			++v2;
			v1 += 20212;
		}
		while ( result > v2 );
	}
	return result;
}

int QueryClientCvarValueCmd2()
{
	char *v0; // eax@2
	int result; // eax@2
	int v2; // ebx@2
	int v3; // esi@3
	char *v4; // eax@6
	char v5; // al@10
	char *v6; // eax@12
	__int32 v7; // edi@12
	char *v8; // eax@12
	int v9; // ebx@12
	int v10; // esi@12
	int v11; // eax@12
	void (__cdecl *v12)(_DWORD); // eax@14
	int v13; // ebx@17
	char endptr; // [sp+4h] [bp-18h]@12
	
	if ( Cmd_Argc() <= 2 )
	{
		v5 = (unsigned int)Cmd_Argv(0);
		return Con_Printf("%s <player name> <cvar> <requestID>\n", v5);
	}
	v0 = Cmd_Argv(3u);
	strtol(v0, 0, 10);
	result = *(&svs + 2);
	v2 = *(&svs + 1);
	if ( result > 0 )
	{
		v3 = 0;
		do
		{
			if ( *(_DWORD *)v2 || *(_DWORD *)(v2 + 12) )
			{
				v4 = Cmd_Argv(1u);
				result = Q_strcasecmp((char *)(v2 + 19432), v4);
				if ( !result )
				{
					if ( *(&svs + 2) > v3 )
					{
						v6 = Cmd_Argv(3u);
						v7 = strtol(v6, 0, 10);
						v8 = Cmd_Argv(2u);
						v9 = *(_DWORD *)(v2 + 19076);
						v10 = (int)v8;
						v11 = NUM_FOR_EDICT(v9);
						if ( v11 > 0 && v11 <= *(&svs + 2) )
						{
							v13 = *(&svs + 1) + 20212 * (v11 - 1) + 116;
							MSG_WriteChar(v13, 58);
							MSG_WriteLong(v13, v7);
							result = (int)MSG_WriteString(v13, v10);
						}
						else
						{
							v12 = (void (__cdecl *)(_DWORD))*(&gNewDLLFunctions + 4);
							if ( v12 )
							{
								endptr = v7;
								v12(v9);
							}
							result = Con_Printf("tried to QueryClientCvarValue a non-client\n", endptr);
						}
					}
					return result;
				}
				result = *(&svs + 2);
			}
			++v3;
			v2 += 20212;
		}
		while ( result > v3 );
	}
	return result;
}
#endif