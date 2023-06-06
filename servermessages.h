#include "Defines.h"
#include "hooked_server.h"
typedef struct clc_func_s
{
	// Opcode
	unsigned char opcode;  
	// Display Name
	char *pszname;         
	// Parse function
	void ( *pfnParse )(int cl);
} clc_func_t;

static clc_func_t sv_clcfuncs[] =
{
	{ clc_bad, "clc_bad", NULL },   // 0
	{ clc_nop, "clc_nop", NULL },   // 1
	{ clc_move, "clc_move", SV_ParseMove }, // 2
	{ clc_stringcmd, "clc_stringcmd", (void(*)(int))SV_ParseStringCommand }, // 3
	{ clc_delta, "clc_delta", (void(*)(int))SV_ParseDelta }, // 4
	{ clc_resourcelist, "clc_resourcelist", (void(*)(int))SV_ParseResourceList }, // 5
	{ clc_tmove, "clc_tmove", NULL }, // 6
	{ clc_fileconsistency, "clc_fileconsistency", (void(*)(int))SV_Parsefileconsistency }, // 7
	{ clc_voicedata, "clc_voicedata", SV_ParseVoiceData }, // 8
	//Steam 
	{ clc_hltv, "clc_hltv", NULL }, // 9 //for some reason the linux versions function name is 'ignorehltv'
	{ clc_cvarvalue, "clc_cvarvalue", (void(*)(int))SV_ParseCvarValue},// SV_ParseCvarValue }, // 10
	{ clc_cvarvalue2, "clc_cvarvalue2", (void(*)(int))SV_ParseCvarValue2},// SV_ParseCvarValue2 }, // 11
	{ (unsigned char)-1, "End of List", NULL }
};
