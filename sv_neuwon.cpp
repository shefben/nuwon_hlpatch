//This is a HLDS patch to add sv_downloadurl, fix bugs on the hlds side

#include "Defines.h"
#include "hooked_funcs.h"
using namespace std;

//replaces half-lifes' cl parse command struct with our own 
//Adds svc_resourcelocation, svc_sendcvar/2
/*
.text:038468AA                                         ; CL_ParseServerMessage+1D9.j
.text:038468AA                 lea     ecx, [esi+esi*2]
.text:038468AD                 shl     ecx, 2
.text:038468B0                 mov     edi, dword_395D850[ecx] ; DATA XREF: 05ADE154.o
.text:038468B6 ; 81:       if ( v10 )
.text:038468B6                 test    edi, edi
.text:038468B8                 jz      short loc_38468E3
*/

int svcmsg = 0;
__declspec(naked) void svc_message_hook() 
{	
	__asm
	{    
		mov svcmsg, ecx
		lea ecx, [esi+esi*2]
		shl ecx, 2
			
		jmp [svc_hook_ret1]
	}
	
}
//replaces half-lifes' server parse command struct with our own
//adds clc_querycvar/2
/*
.text:01D948E8 ; 43:     if ( result < 0 || result > 11 )
.
.text:01D948E8                 test    eax, eax
.text:01D948EA                 jl      short loc_1D94921
.text:01D948EC ; 44:       break;
.text:01D948EC                 cmp     eax, 11
.text:01D948EF                 jg      short loc_1D94921
9 bits 
.text:01D948F1 ; 45:     v9 = dword_1E46880[3 * result];
.text:01D948F1                 lea     ecx, [eax+eax*2]
.
.
. 03CC48E8					   test    eax, eax
.text:03CC48EA                 jl      short loc_3CC4921
.text:03CC48EC                 cmp     eax, 9
.text:03CC48EF                 jg      short loc_3CC4921
.text:03CC48F1                 lea     ecx, [eax+eax*2]
.*/
int clcmsg = 0;
int arg1 = 0;
__declspec(naked) void clc_message_hook() 
{	
	__asm
	{  
		mov clcmsg, eax
		mov arg1, esi
		test eax, eax
	}
	if (clcmsg <=0)
		__asm jmp [clc_hook_ret1]
	if(clcmsg >9){
		 switch(clcmsg)
		 {
			 case 10: //cvarquery
				 SV_ParseCvarValue(arg1);
			 case 11: //cvarquery2
				 SV_ParseCvarValue2(arg1);
				 __asm jmp [clc_hook_ret3]
				 break;
			 default:
				 __asm jmp [clc_hook_ret1]
				 break;
		 }
		}
	__asm
	{
			
		jmp [clc_hook_ret2]
	}
	
} 


//SV_sendresourcelist - added sv_downloadurl to this packet (same way as steam)
//0x01D809F0
/*
.text:038B0A1C                 push    0
.text:038B0A1E                 push    ebx
.text:038B0A1F                 call    MSG_WriteLong
.text:038B0A24 ; 12:   MSG_WriteByte(a1, 43);
.text:038B0A24                 push    2Bh
*/

int a1;
__declspec(naked) void sendResource_Hook() 
{	
	
	__asm{ 
		push ebx
		call  MSG_WriteLong
		mov a1, ebx
		pusha
	}
 	if (downloadurl.string[0])
 	{	
		MSG_WriteByte(a1, 56);
		MSG_WriteString(a1, reinterpret_cast<int>(downloadurl.string));
 	}
	__asm{
		popa

		jmp [resourcehook_ret]
	}
}

/*
.text:03C534CB                   mov     edx, com_ignorecolons
.text:03C534D1                   cmp     edx, ebx
.*/
//int command;
__declspec(naked) void dlurl_token() 
{	
// 	__asm
// 	{    //if you want to do any other calls to other functions, etc and don't need the registers/stack anymore push it
// 		mov command, eax
// 	}
	// 	if (strstr (command,"http"))
	// 		com_ignorecolons = true;
	// 	else
	// 		com_ignorecolons = false;
	// 
	__asm
	{
			cmp  edx, ebx
			jmp [dlurl_ret]
	}
}

/*
.text:049A7066 008 A3 AC 83 C4 04                                                  mov     wadpath, eax
.text:049A706B
.text:049A706B                                                     loc_49A706B:                            ; CODE XREF: Mod_LoadEntities+5B?j
.text:049A706B                                                                                             ; Mod_LoadEntities+6B?j ...
.text:049A706B 008 5E                                                              pop     esi
.text:049A706C 004 5D                                                              pop     ebp
*/
unsigned char finalstr[100];
char *token, *token2, *saveptr, *saveptr2;
int count = 0;
char* fullpath;

__declspec(naked) void addresource_fix() 
{	
	__asm
	{
		//mov   eax, wadpath
		
		mov fullpath, eax
			mov wadpath, eax

	pushad

	}
//	fullpath = wadpath;

	Con_Printf((char*)fullpath);
			//	PF_precache_generic_I((char*)fullpath);

	token = strtok_r((char*)fullpath, ";", &saveptr);
// Con_Printf("testomg testing");
	Con_Printf("\ncount=0");
	PF_precache_generic_I((char*)"valve\\dm_lego_bricks.wad");


    while(token != NULL) {
        token2 = strtok_r(token, "\\", &saveptr2);
        while(token2 != NULL) {
            count++;
            if(count ==3){
//                strcat(finalstr,"\\");
                strcat((char*)finalstr,token2);
				strcat((char*)finalstr,"\\");
								Con_Printf("\ncount=3");
				Con_Printf((char*)finalstr);

            }
			else if(count ==4){
				strcat((char*)finalstr,token2);
				Con_Printf("\ncount=4");
				Con_Printf((char*)finalstr);

			//	PF_precache_generic_I((char*)finalstr);
				memset(finalstr,NULL,70);
			}
            token2 = strtok_r(NULL, "\\", &saveptr2);
			
        }
        count = 0;
        token = strtok_r(NULL, ";", &saveptr);
    }
	fullpath = 0;
	token = NULL;
	token2 = NULL;
	memset(&finalstr[0], 0, sizeof(finalstr));
	saveptr = NULL;
	saveptr2 = NULL;
	__asm{
			popad

pop esi
pop ebp
retn
	//		jmp [svloadwad_ret]
	}
}

void start()
{
	Con_Printf("\nNeuWON Server Module Loaded Successfully [1.2v]\n");
	//	Con_Printf("\nass\n");
	Cvar_RegisterVariable(reinterpret_cast<int*>(&downloadurl));
	//	Cvar_RegisterVariable(reinterpret_cast<int*>(&AllowFragDL));
//	placeJMP((BYTE*)svloadwad, (DWORD)addresource_fix, 5); //fix to add wads to the resource list for client downloads
	
//	placeJMP((BYTE*)dlurl_fix, (DWORD)dlurl_token, 6); //fix to allow semi colon for dlurc
//	placeJMP((BYTE*)clc_hook, (DWORD)clc_message_hook, 7); //new clc messages
//	placeJMP((BYTE*)svc_hook, (DWORD)svc_message_hook, 6); //new svc messages
	placeJMP((BYTE*)resourcehook, (DWORD)sendResource_Hook, 6); //send download url if available		
	
}

__declspec(naked) void master_init() 
{	
	__asm {
		pusha
	}
	
	start();
	__asm
	{
		popa
			inc eax
			mov edi, 1
			jmp [masterinit_ret]
	}
}

#ifdef cl_hlds
/*
.text:038B5097 130 C7 45 F8 00 00 00 00                                            mov     dword ptr [ebp-8], 0
.text:038B509E      
*/

__declspec(naked) void auth_init() 
{	
	__asm {
		pusha
	}
	
	start();
	__asm
	{
		popa
		mov dword ptr [ebp-8], 0
		jmp [authinit_ret]
	}
}
#endif
BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{	
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		base_address= (DWORD)GetModuleHandle("swds.dll"); 
#ifdef cl_hlds
		//placeJMP((BYTE*)authinit, (DWORD)auth_init, 7); //fix to add wads to the resource list for client downloads	
start();
#else
		placeJMP((BYTE*)masterinit, (DWORD)master_init, 6); //fix to add wads to the resource list for client downloads	
#endif
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
extern "C"
{	
	void __cdecl export(void)
	{	


		// THIS IS JUST A PLACE HOLDER SO THE DLL LOADS WITH HL
	}
}