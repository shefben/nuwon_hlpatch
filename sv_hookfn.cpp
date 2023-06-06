#include <string.h>
#include "hooked_funcs.h"


/*
.text:01DA0239 ; 38:     if ( !j__strstr(wadName, "pldecal") )
.text:01DA0239                 lea     eax, [esp+648h+wadName] ; Load Effective Address
.text:01DA0240                 push    offset aPldecal ; char *
.text:01DA0245                 push    eax             ; char *
.text:01DA0246 ; 39:       break;
.text:01DA0246                 call    j__strstr       ; Call Procedure
.text:01DA024B                 add     esp, 30h        ; Add  //RETURN HERE
.text:01DA024E                 test    eax, eax        ; Logical Compare
.text:01DA0250                 jz      short loc_1DA025C ; Jump if Zero (ZF=1)
*/
int wadName;
__declspec(naked) void Host_tempdecal_fix()
	{
		__asm
			{
				pushad
				lea     eax, [esp+144h] 
				mov wadName, eax
			}

		if (!strstr((const char*)wadName, "pldecal") && !strstr((const char*)wadName, "tempdecal"))
			{
				__asm
					{
						popad
						jmp [host_tempdecal_ret]
					}
			}
		else
			{
				__asm
					{
						popad
						jmp [host_tempdecal_ret2]
					}
			}
 }

/*
.text:01DA03BB                 push    edx             ; _DWORD
.text:01DA03BC                 call    LittleLong      ; Indirect Call Near Procedure
.text:01DA03BC
.text:01DA03C2                 mov     [esp+624h+wadinfo.numlumps], eax

//Wadpath
.text:01DA03A5                 lea     ecx, [esp+18h]  ; Load Effective Address
.text:01DA03A9                 push    ecx             ; char
*/


__declspec(naked) void Host_wad_reslist_fix()
 {
	__asm
		{
			push edx
			call LittleLong
			pushad
			lea     ecx, [esp+18h]
			push ecx
			call PF_precache_generic_I
			popad
			jmp [host_wadfix_ret]
		}
 }