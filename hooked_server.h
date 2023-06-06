#ifndef hooked_server_h__
#define hooked_server_h__
#include "Defines.h"


#ifdef client

DWORD parsemove		= (DWORD)base_address + 0x199C11F;
DWORD stringcmd		= (DWORD)base_address + 0x199BF9F;
DWORD delta			= (DWORD)base_address + 0x199BFFF;
DWORD svreslist		= (DWORD)base_address + 0x1998EA0;
DWORD fileconsist	= (DWORD)base_address + 0x1999160;
DWORD voicedata		= (DWORD)base_address + 0x199C59F;

#endif

#ifdef sv_hlds

DWORD parsemove		= (DWORD)base_address + 0x9425F;
DWORD stringcmd		= (DWORD)base_address + 0x940EF;
DWORD delta			= (DWORD)base_address + 0x9414F;
DWORD svreslist		= (DWORD)base_address + 0x91080;
DWORD fileconsist	= (DWORD)base_address + 0x91310;
DWORD voicedata		= (DWORD)base_address + 0x946DF;

#endif

#ifdef cl_hlds

DWORD parsemove		= (DWORD)base_address + 0x0;
DWORD stringcmd		= (DWORD)base_address + 0x0;
DWORD delta			= (DWORD)base_address + 0x0;
DWORD svreslist		= (DWORD)base_address + 0x0;
DWORD fileconsist	= (DWORD)base_address + 0x0;
DWORD voicedata		= (DWORD)base_address + 0x0;

#endif

typedef void	   (*sv_parsemove_orig)	(int);
sv_parsemove_orig SV_ParseMove							= reinterpret_cast<sv_parsemove_orig>(parsemove);

typedef void	   (*sv_parsestringcmd_orig)(void);
sv_parsestringcmd_orig SV_ParseStringCommand			= reinterpret_cast<sv_parsestringcmd_orig>(stringcmd);

typedef signed int (*sv_parsedelta_orig)(void);
sv_parsedelta_orig SV_ParseDelta						= reinterpret_cast<sv_parsedelta_orig>(delta);

typedef int		   (*SV_ParseResourceList_orig)(void);
SV_ParseResourceList_orig SV_ParseResourceList			= reinterpret_cast<SV_ParseResourceList_orig>(svreslist);

typedef int		   (*SV_Parsefileconsistency_orig)(void);
SV_Parsefileconsistency_orig SV_Parsefileconsistency	= reinterpret_cast<SV_Parsefileconsistency_orig>(fileconsist);

typedef void	   (*SV_ParseVoiceData_orig)(int);
SV_ParseVoiceData_orig SV_ParseVoiceData				= reinterpret_cast<SV_ParseVoiceData_orig>(voicedata);

#endif // hooked_server_h__
