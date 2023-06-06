

#ifndef hooked_funcs_h__
#define hooked_funcs_h__

//Contains all the basic functions

#include "Defines.h"

#ifdef sv_hlds

static DWORD condprintf					= (DWORD)base_address + 0x24ED0;
static DWORD conprintf					= (DWORD)base_address + 0x24DB0;

static DWORD msgwritebyte				= (DWORD)base_address + 0x22140;		
static DWORD msgwritestring				= (DWORD)base_address + 0x22200;	
static DWORD msgwritelong				= (DWORD)base_address + 0x221A0;		
static DWORD msgreadstring				= (DWORD)base_address + 0x22F40;		
static DWORD msgreadlong				= (DWORD)base_address + 0x22E40;
		
static DWORD findvar					= (DWORD)base_address + 0x265E0;			
static DWORD strcasecmp					= (DWORD)base_address + 0x21B40;		
static DWORD littlelong_addr			= (DWORD)base_address + 0xC3F50;
static DWORD svaddresource				= (DWORD)base_address + 0x86B40;
static DWORD PF_precache_generic_I_addr = (DWORD)base_address + 0x542B0; 

#endif // sv_hlds

#ifdef cl_hlds

static DWORD conprintf					= (DWORD)base_address + 0x24230;
static DWORD condprintf					= (DWORD)base_address + 0x24350;

static DWORD msgwritebyte				= (DWORD)base_address + 0x215F0;  
static DWORD msgwritestring				= (DWORD)base_address + 0x216B0;
static DWORD msgwritelong				= (DWORD)base_address + 0x21650;
static DWORD msgreadstring				= (DWORD)base_address + 0x223F0;
static DWORD msgreadlong				= (DWORD)base_address + 0x222F0;

static DWORD findvar					= (DWORD)base_address + 0x25A60;			
static DWORD strcasecmp					= (DWORD)base_address + 0x20FF0;
static DWORD littlelong_addr			= (DWORD)base_address + 0x0;
static DWORD svaddresource				= (DWORD)base_address + 0x0;
static DWORD PF_precache_generic_I_addr = (DWORD)base_address + 0x0;

#endif // cl_hlds

#ifdef client

static DWORD condprintf					= (DWORD)base_address + 0x1923DD0;
static DWORD conprintf					= (DWORD)base_address + 0x1923C50;		

static DWORD msgwritebyte				= (DWORD)base_address + 0x191FDF0;	
static DWORD msgwritestring				= (DWORD)base_address + 0x191FEB0;
static DWORD msgwritelong				= (DWORD)base_address + 0x1921470;	
static DWORD msgreadstring				= (DWORD)base_address + 0x1920BE0;
static DWORD msgreadlong				= (DWORD)base_address + 0x1920AE0;
	
static DWORD findvar					= (DWORD)base_address + 0x1925880;
static DWORD strcasecmp					= (DWORD)base_address + 0x191F880;		
static DWORD littlelong_addr			= (DWORD)base_address + 0x2B7B890;
static DWORD svaddresource				= (DWORD)base_address + 0x0;
static DWORD PF_precache_generic_I_addr = (DWORD)base_address + 0x1963100;
static DWORD cldisconnect				= (DWORD)base_address + 0x190E810;

void		Host_tempdecal_fix();
void		Host_wad_reslist_fix();

typedef int (*CL_Disconnect_orig)();
static		CL_Disconnect_orig CL_Disconnect     = reinterpret_cast<CL_Disconnect_orig>(cldisconnect);

#endif // client

//////////////////////////////////////////////////////////////////////////
// Below contains all of the functions we need access to.
//////////////////////////////////////////////////////////////////////////

typedef int (*LittleLong_orig)(DWORD);
static		LittleLong_orig LittleLong		     = reinterpret_cast<LittleLong_orig>(littlelong_addr);

typedef int (*SV_AddResource_orig)(int, int, int, char, int);
static		SV_AddResource_orig SV_AddResource   = reinterpret_cast<SV_AddResource_orig>(svaddresource);

typedef int (*Cvar_FindVar_orig)(int);
static		Cvar_FindVar_orig Cvar_FindVar	     = reinterpret_cast<Cvar_FindVar_orig>(findvar);

typedef char (*Con_Printf_orig)(char*, ...);
static		Con_Printf_orig Con_Printf		     = reinterpret_cast<Con_Printf_orig>(conprintf);

typedef void (*Con_DPrintf_orig)(char*, ...);
static		Con_DPrintf_orig Con_DPrintf	     = reinterpret_cast<Con_DPrintf_orig>(condprintf);

typedef signed int (*Q_strncasecmp_orig)(int, int);
static		Q_strncasecmp_orig Q_strncasecmp     = reinterpret_cast<Q_strncasecmp_orig>(strcasecmp);

//////////////////////////////////////////////////////////////////////////
//message write functions
//////////////////////////////////////////////////////////////////////////

typedef int (*MSG_WriteByte_orig)(int, char);
static		MSG_WriteByte_orig MSG_WriteByte     = reinterpret_cast<MSG_WriteByte_orig>(msgwritebyte);

typedef int (*MSG_WriteString_orig)(int, int);
static		MSG_WriteString_orig MSG_WriteString = reinterpret_cast<MSG_WriteString_orig>(msgwritestring);

typedef int (*MSG_WriteLong_orig)(int, signed int);
static		MSG_WriteLong_orig MSG_WriteLong	 = reinterpret_cast<MSG_WriteLong_orig>(msgwritelong);

//////////////////////////////////////////////////////////////////////////
//message read functions.
//////////////////////////////////////////////////////////////////////////

typedef int (*MSG_ReadString_orig)();
static		MSG_ReadString_orig MSG_ReadString	 = reinterpret_cast<MSG_ReadString_orig>(msgreadstring);

typedef int (*MSG_ReadLong_orig)();
static		MSG_ReadLong_orig MSG_ReadLong		 = reinterpret_cast<MSG_ReadLong_orig>(msgreadlong);

typedef int (*PF_precache_generic_I_orig)(char *);
static		PF_precache_generic_I_orig PF_precache_generic_I = reinterpret_cast<PF_precache_generic_I_orig>(PF_precache_generic_I_addr);

typedef int (*Cvar_RegisterVariable_orig)(int*);
static		Cvar_RegisterVariable_orig Cvar_RegisterVariable = reinterpret_cast<Cvar_RegisterVariable_orig>(registervar);


#endif // hooked_funcs_h__


