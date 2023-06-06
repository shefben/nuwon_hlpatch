/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** vid_mode.h
** Contains Client ONLY Definitions
** Author: Benjamin Shefte
** -------------------------------------------------------------------------*/
#ifndef hooked_client_h__
#define hooked_client_h__
#include "defines.h"

//////////////////////////////////////////////////////////////////////////
// Address's we are either hooking or just retrieving values from
//////////////////////////////////////////////////////////////////////////
static int dlprogress;
static int totalfileprogress	= 0;
static int m_totalRequests		= -1;
static int m_failedRequests		= 0;
static int m_filesCompleted		= 0;

static DWORD cl_download_ingame		   = (DWORD)base_address + 0x19E7E60; 
static DWORD cl_allow_download		   = (DWORD)base_address + 0x19E7DE8;
static DWORD cls_0					   = (DWORD)base_address + 0x2BCCD80;
static DWORD unknown_1				   = (DWORD)base_address + 0x2BD1228;
static DWORD unknown_2				   = (DWORD)base_address + 0x2BD0EF8;
static DWORD com_gamedir			   = (DWORD)base_address + 0x2B7B8A0;
static DWORD ipaddress				   = (DWORD)base_address + 0x2BD02A4; // Contains the server address the client is connected to
static DWORD client_state			   = (DWORD)base_address + 0x2BD02A0; // checks which state client is in, connecting, downloading etcetc..
static DWORD sv_active				   = (DWORD)base_address + 0x2029200; // Used to check if client is the server
static DWORD cl_move_to_list		   = (DWORD)base_address + 0x1911DB0;
static DWORD net_updateprogrss		   = (DWORD)base_address + 0x1951AC0;
static DWORD gDownloadFile			   = (DWORD)base_address + 0x2628100; 
static DWORD net_drawslider			   = (DWORD)base_address + 0x19F9BCC; 
static DWORD scr_downloading		   = (DWORD)base_address + 0x19F47D0; 
static DWORD Cvar_Set_Address		   = (DWORD)base_address + 0x1925C70;
static DWORD scr_vrect				   = (DWORD)base_address + 0x262C1A8;
static DWORD dword_2A2C1A0			   = (DWORD)base_address + 0x262C1A0;
static DWORD scr_drawloading		   = (DWORD)base_address + 0x262C1D8;
static DWORD NET_FillRect_addr		   = (DWORD)base_address + 0x19545C0;
static DWORD SCR_UpdateScreen_addr     = (DWORD)base_address + 0x193A2F0; 
static DWORD NET_DrawString_addr	   = (DWORD)base_address + 0x1954600;
static DWORD GLFinishHud_addr	       = (DWORD)base_address + 0x193E0F0;
static DWORD SCR_ConnectMsg_addr	   = (DWORD)base_address + 0x193A6A0;
static DWORD SCR_DrawDownloadText_addr = (DWORD)base_address + 0x193A950;
static DWORD dword_2FCCD88			   = (DWORD)base_address + 0x2BCCD88;
static DWORD byte_1E120A8			   = (DWORD)base_address + 0x1A120A8;
static DWORD dword_2FD0EF8			   = (DWORD)base_address + 0x2BD0EF8;
static DWORD dword_2FCCD80			   = (DWORD)base_address + 0x2BCCD80;
static DWORD ResourcePrecache_return   = (DWORD)base_address + 0x19122A4;
static DWORD ResourcePrecache_addr	   = (DWORD)base_address + 0x191229E;
static DWORD PrecacheResources_addr	   = (DWORD)base_address + 0x190DBA0;
static DWORD RegisterResources_addy    = (DWORD)base_address + 0x1911DB0;
static DWORD Cbuf_AddText_addr	       = (DWORD)base_address + 0x191DBB0;

//////////////////////////////////////////////////////////////////////////
// The added functions
//////////////////////////////////////////////////////////////////////////
void CL_ParseResourceLocation();
void CL_Send_CvarValue();
void CL_Send_CvarValue2();
//////////////////////////////////////////////////////////////////////////

void CL_UpdateProgressBar();
int  IsSafeFileToDownload(char *a1);

typedef void (*xcommand_t)(void);

typedef void(*Con_Command_orig)(char*);                          
static Con_Command_orig Con_Command = reinterpret_cast<Con_Command_orig>(con_command_addr);
//void __usercall Cmd_AddCommand(int, int)
typedef void(*Cmd_AddCommand_orig)(char*, xcommand_t);                          
static Cmd_AddCommand_orig Cmd_AddCommand = reinterpret_cast<Cmd_AddCommand_orig>(cmd_addcmd);

typedef void(*Cbuf_AddText_orig)(char *a1);                        
static Cbuf_AddText_orig Cbuf_AddText = reinterpret_cast<Cbuf_AddText_orig>(Cbuf_AddText_addr); 

typedef signed int(*CL_PrecacheResources_orig)();                          
static CL_PrecacheResources_orig CL_PrecacheResources = reinterpret_cast<CL_PrecacheResources_orig>(PrecacheResources_addr);

typedef signed int(*CL_RegisterResources_orig)(int);                          
static CL_RegisterResources_orig CL_RegisterResources = reinterpret_cast<CL_RegisterResources_orig>(RegisterResources_addy);

typedef int(*sub_1D51A30_orig)(int);                          
static sub_1D51A30_orig sub_1D51A30 = reinterpret_cast<sub_1D51A30_orig>((DWORD)base_address + (DWORD)0x1951A30);

typedef int(*sub_1D51A60_orig)(int);                          
static sub_1D51A60_orig sub_1D51A60 = reinterpret_cast<sub_1D51A60_orig>((DWORD)base_address + (DWORD)0x1951A60);

typedef int(*SCR_DrawDownloadText_orig)(signed int, int);                          
static SCR_DrawDownloadText_orig SCR_DrawDownloadText = reinterpret_cast<SCR_DrawDownloadText_orig>(SCR_DrawDownloadText_addr);

typedef int(*SCR_ConnectMsg_orig)();                          
static SCR_ConnectMsg_orig SCR_ConnectMsg = reinterpret_cast<SCR_ConnectMsg_orig>(SCR_ConnectMsg_addr);

typedef int(*GLFinishHud_orig)();                          
static GLFinishHud_orig GLFinishHud = reinterpret_cast<GLFinishHud_orig>(GLFinishHud_addr);

typedef void(*Cvar_Set_orig)(int a1, int a2);                          
static Cvar_Set_orig Cvar_Set = reinterpret_cast<Cvar_Set_orig>(Cvar_Set_Address);

typedef int(*NET_FillRect_orig)(int a1, int a2, unsigned __int8 a3);                          
static NET_FillRect_orig NET_FillRect = reinterpret_cast<NET_FillRect_orig>(NET_FillRect_addr);

typedef int(*NET_DrawString_orig)(int arg0, int arg4, int _28, float argC, float arg10, float arg14, char *arg18, ...);                          
static NET_DrawString_orig NET_DrawString = reinterpret_cast<NET_DrawString_orig>(NET_DrawString_addr);

typedef void(*SCR_UpdateScreen_orig)(int);                          
static SCR_UpdateScreen_orig SCR_UpdateScreen = reinterpret_cast<SCR_UpdateScreen_orig>(SCR_UpdateScreen);

typedef int(*NET_AdrToString_orig)(int a1, int a2, int a3, int a4, unsigned int a5);                          
static NET_AdrToString_orig NET_AdrToString = reinterpret_cast<NET_AdrToString_orig>((DWORD)base_address + (DWORD)0x1951FC0);

typedef void(*CL_MoveToOnHandList_orig)(int a1);                          
static CL_MoveToOnHandList_orig CL_MoveToOnHandList = reinterpret_cast<CL_MoveToOnHandList_orig>(cl_move_to_list);

typedef void(*Netchan_UpdateProgress_orig)(int a1);                          
static Netchan_UpdateProgress_orig Netchan_UpdateProgress = reinterpret_cast<Netchan_UpdateProgress_orig>(net_updateprogrss);

typedef int(*SCR_DrawDownloadtext_orig)(signed int a1, int a2);                          
static SCR_DrawDownloadtext_orig SCR_DrawDownloadingText = reinterpret_cast<SCR_DrawDownloadtext_orig>((DWORD)base_address + (DWORD)0x193A950);
   
typedef void(*SCR_DrawDownloadtext1_orig)();                          
static SCR_DrawDownloadtext1_orig SCR_DrawDownloadingText1 = reinterpret_cast<SCR_DrawDownloadtext1_orig>((DWORD)base_address + (DWORD)0x193AAC0);   
                          
typedef void(*Host_CheckConnectionFailure_orig)();
static Host_CheckConnectionFailure_orig Host_CheckConnectionFailure = reinterpret_cast<Host_CheckConnectionFailure_orig>((DWORD)base_address + (DWORD)0x1945710);

typedef int(*ClientDLL_CAM_Think_orig)();
static ClientDLL_CAM_Think_orig ClientDLL_CAM_Think = reinterpret_cast<ClientDLL_CAM_Think_orig>((DWORD)base_address + (DWORD)0x19028B0);

typedef int(*CL_MoveSpectatorCamera_orig)();
static CL_MoveSpectatorCamera_orig CL_MoveSpectatorCamera = reinterpret_cast<CL_MoveSpectatorCamera_orig>((DWORD)base_address + (DWORD)0x1916E30);

typedef void(*CL_UpdateSoundFade_orig)();
static CL_UpdateSoundFade_orig CL_UpdateSoundFade = reinterpret_cast<CL_UpdateSoundFade_orig>((DWORD)base_address + (DWORD)0x190C160);

//////////////////////////////////////////////////////////////////////////
// General functions so I don't need to recode/make the dll any bigger than need be
//////////////////////////////////////////////////////////////////////////

typedef int (*FS_FileExists_orig)(int);
static FS_FileExists_orig FS_FileExists = reinterpret_cast<FS_FileExists_orig>((DWORD)base_address + (DWORD)0x1921E80);

typedef int  (*COM_CreatePath_orig)(char);
static COM_CreatePath_orig COM_CreatePath = reinterpret_cast<COM_CreatePath_orig>((DWORD)base_address + (DWORD)0x1921B70);
 
//////////////////////////////////////////////////////////////////////////
// msg function's
//////////////////////////////////////////////////////////////////////////

typedef void (*CL_Parse_disconnect_orig)(void);
static CL_Parse_disconnect_orig CL_Parse_disconnect = reinterpret_cast<CL_Parse_disconnect_orig>((DWORD)base_address + (DWORD)0x1914430);

typedef int (*CL_Parse_event_orig)();
static CL_Parse_event_orig CL_Parse_event = reinterpret_cast<CL_Parse_event_orig>((DWORD)base_address + (DWORD)0x1914CA0);

typedef int (*CL_Parse_version_orig)();
static CL_Parse_version_orig CL_Parse_version = reinterpret_cast<CL_Parse_version_orig>((DWORD)base_address + (DWORD)0x1914450);

typedef int (*CL_Parse_setview_orig)();
static CL_Parse_setview_orig CL_Parse_setview = reinterpret_cast<CL_Parse_setview_orig>((DWORD)base_address + (DWORD)0x19144C0);

typedef void (*CL_Parse_sound_orig)();
static CL_Parse_sound_orig CL_Parse_sound = reinterpret_cast<CL_Parse_sound_orig>((DWORD)base_address + (DWORD)0x1911260);

typedef void (*CL_Parse_time_orig)();
static CL_Parse_time_orig CL_Parse_time = reinterpret_cast<CL_Parse_time_orig>((DWORD)base_address + (DWORD)0x1914330);

typedef void (*CL_Parse_print_orig)();
static CL_Parse_print_orig CL_Parse_print = reinterpret_cast<CL_Parse_print_orig>((DWORD)base_address + (DWORD)0x19144D0);

typedef void (*CL_Parse_stufftext_orig)();
static CL_Parse_stufftext_orig CL_Parse_stufftext = reinterpret_cast<CL_Parse_stufftext_orig>((DWORD)base_address + (DWORD)0x1914500);

typedef void (*CL_Parse_setangle_orig)();
static CL_Parse_setangle_orig CL_Parse_setangle = reinterpret_cast<CL_Parse_setangle_orig>((DWORD)base_address + (DWORD)0x1914530);

typedef void (*CL_Parse_serverinfo_orig)();
static CL_Parse_serverinfo_orig CL_Parse_serverinfo = reinterpret_cast<CL_Parse_serverinfo_orig>((DWORD)base_address + (DWORD)0x1914510);

typedef int (*CL_Parse_lightstyle_orig)();
static CL_Parse_lightstyle_orig CL_Parse_lightstyle = reinterpret_cast<CL_Parse_lightstyle_orig>((DWORD)base_address + (DWORD)0x19145C0);

typedef char (*CL_Parse_updateuserinfo_orig)();
static CL_Parse_updateuserinfo_orig CL_Parse_updateuserinfo = reinterpret_cast<CL_Parse_updateuserinfo_orig>((DWORD)base_address + (DWORD)0x19147D0);

typedef int (*CL_Parse_deltadescription_orig)();
static CL_Parse_deltadescription_orig CL_Parse_deltadescription = reinterpret_cast<CL_Parse_deltadescription_orig>((DWORD)base_address + (DWORD)0x1913740); //14

typedef int (*CL_Parse_clientdata_orig)();
static CL_Parse_clientdata_orig CL_Parse_clientdata = reinterpret_cast<CL_Parse_clientdata_orig>((DWORD)base_address + (DWORD)0x1914420);

typedef int (*CL_Parse_stopsound_orig)();
static CL_Parse_stopsound_orig CL_Parse_stopsound = reinterpret_cast<CL_Parse_stopsound_orig>((DWORD)base_address + (DWORD)0x1914620);

typedef int (*CL_Parse_pings_orig)();
static CL_Parse_pings_orig CL_Parse_pings = reinterpret_cast<CL_Parse_pings_orig>((DWORD)base_address + (DWORD)0x1914FC0);

typedef int (*CL_Parse_particle_orig)();
static CL_Parse_particle_orig CL_Parse_particle = reinterpret_cast<CL_Parse_particle_orig>((DWORD)base_address + (DWORD)0x1914640);

typedef void (*CL_Parse_damage_orig)();
static CL_Parse_damage_orig CL_Parse_damage = reinterpret_cast<CL_Parse_damage_orig>((DWORD)base_address + (DWORD)0x19147E0);

typedef void (*CL_Parse_spawnstatic_orig)();
static CL_Parse_spawnstatic_orig CL_Parse_spawnstatic = reinterpret_cast<CL_Parse_spawnstatic_orig>((DWORD)base_address + (DWORD)0x1914660);

typedef int (*CL_Parse_eventreliable_orig)();
static CL_Parse_eventreliable_orig CL_Parse_eventreliable = reinterpret_cast<CL_Parse_eventreliable_orig>((DWORD)base_address + (DWORD)0x1914EE0);

typedef int (*CL_Parse_spawnbaseline_orig)();
static CL_Parse_spawnbaseline_orig CL_Parse_spawnbaseline = reinterpret_cast<CL_Parse_spawnbaseline_orig>((DWORD)base_address + (DWORD)0x1914650);

typedef void (*CL_Parse_tempentity_orig)();
static CL_Parse_tempentity_orig CL_Parse_tempentity = reinterpret_cast<CL_Parse_tempentity_orig>((DWORD)base_address + (DWORD)0x1914670);

typedef int (*CL_Parse_setpause_orig)();
static CL_Parse_setpause_orig CL_Parse_setpause = reinterpret_cast<CL_Parse_setpause_orig>((DWORD)base_address + (DWORD)0x1914680);

typedef int (*CL_Parse_signonnum_orig)();
static CL_Parse_signonnum_orig CL_Parse_signonnum = reinterpret_cast<CL_Parse_signonnum_orig>((DWORD)base_address + (DWORD)0x19146A0);

typedef char (*CL_Parse_centerprint_orig)();
static CL_Parse_centerprint_orig CL_Parse_centerprint = reinterpret_cast<CL_Parse_centerprint_orig>((DWORD)base_address + (DWORD)0x19144F0);

typedef void (*CL_Parse_killedmonster_orig)();
static CL_Parse_killedmonster_orig CL_Parse_killedmonster = reinterpret_cast<CL_Parse_killedmonster_orig>((DWORD)base_address + (DWORD)0x19147F0);

typedef void (*CL_Parse_foundsecret_orig)();
static CL_Parse_foundsecret_orig CL_Parse_foundsecret = reinterpret_cast<CL_Parse_foundsecret_orig>((DWORD)base_address + (DWORD)0x1914800);

typedef int (*CL_Parse_spawnstaticsound_orig)();
static CL_Parse_spawnstaticsound_orig CL_Parse_spawnstaticsound = reinterpret_cast<CL_Parse_spawnstaticsound_orig>((DWORD)base_address + (DWORD)0x19146D0);

typedef signed int (*CL_Parse_intermission_orig)();
static CL_Parse_intermission_orig CL_Parse_intermission = reinterpret_cast<CL_Parse_intermission_orig>((DWORD)base_address + (DWORD)0x1914810);

typedef char (*CL_Parse_finale_orig)();
static CL_Parse_finale_orig CL_Parse_finale = reinterpret_cast<CL_Parse_finale_orig>((DWORD)base_address + (DWORD)0x1914820);

typedef signed int (*CL_Parse_cdtrack_orig)();
static CL_Parse_cdtrack_orig CL_Parse_cdtrack = reinterpret_cast<CL_Parse_cdtrack_orig>((DWORD)base_address + (DWORD)0x19146E0);

typedef int (*CL_Parse_restore_orig)();
static CL_Parse_restore_orig CL_Parse_restore = reinterpret_cast<CL_Parse_restore_orig>((DWORD)base_address + (DWORD)0x1914740);

typedef char (*CL_Parse_cutscene_orig)();
static CL_Parse_cutscene_orig CL_Parse_cutscene = reinterpret_cast<CL_Parse_cutscene_orig>((DWORD)base_address + (DWORD)0x1914850);

typedef signed int (*CL_Parse_weaponanim_orig)();
static CL_Parse_weaponanim_orig CL_Parse_weaponanim = reinterpret_cast<CL_Parse_weaponanim_orig>((DWORD)base_address + (DWORD)0x1914750);

typedef int (*CL_Parse_decalname_orig)();
static CL_Parse_decalname_orig CL_Parse_decalname = reinterpret_cast<CL_Parse_decalname_orig>((DWORD)base_address + (DWORD)0x1914770);

typedef void (*CL_Parse_roomtype_orig)();
static CL_Parse_roomtype_orig CL_Parse_roomtype = reinterpret_cast<CL_Parse_roomtype_orig>((DWORD)base_address + (DWORD)0x1914790);

typedef void (*CL_Parse_addangle_orig)();
static CL_Parse_addangle_orig CL_Parse_addangle = reinterpret_cast<CL_Parse_addangle_orig>((DWORD)base_address + (DWORD)0x1914550);

typedef int (*CL_Parse_newusermsg_orig)();
static CL_Parse_newusermsg_orig CL_Parse_newusermsg = reinterpret_cast<CL_Parse_newusermsg_orig>((DWORD)base_address + (DWORD)0x19147C0);

typedef int (*CL_Parse_packetentities_orig)();
static CL_Parse_packetentities_orig CL_Parse_packetentities = reinterpret_cast<CL_Parse_packetentities_orig>((DWORD)base_address + (DWORD)0x1914880);

typedef int (*CL_Parse_deltapacketentities_orig)();
static CL_Parse_deltapacketentities_orig CL_Parse_deltapacketentities = reinterpret_cast<CL_Parse_deltapacketentities_orig>((DWORD)base_address + (DWORD)0x19148A0);

typedef int (*CL_Parse_choke_orig)();
static CL_Parse_choke_orig CL_Parse_choke = reinterpret_cast<CL_Parse_choke_orig>((DWORD)base_address + (DWORD)0x19148C0);

typedef void (*CL_Parse_resourcelist_orig)();
static CL_Parse_resourcelist_orig CL_Parse_resourcelist = reinterpret_cast<CL_Parse_resourcelist_orig>((DWORD)base_address + (DWORD)0x19148F0);

typedef int (*CL_Parse_newmovevars_orig)();
static CL_Parse_newmovevars_orig CL_Parse_newmovevars = reinterpret_cast<CL_Parse_newmovevars_orig>((DWORD)base_address + (DWORD)0x1914900);

typedef int (*CL_Parse_resourcerequest_orig)();
static CL_Parse_resourcerequest_orig CL_Parse_resourcerequest = reinterpret_cast<CL_Parse_resourcerequest_orig>((DWORD)base_address + (DWORD)0x1914910);

typedef void (*CL_Parse_customization_orig)();
static CL_Parse_customization_orig CL_Parse_customization = reinterpret_cast<CL_Parse_customization_orig>((DWORD)base_address + (DWORD)0x1914920);

typedef signed int (*CL_Parse_crosshairangle_orig)();
static CL_Parse_crosshairangle_orig CL_Parse_crosshairangle = reinterpret_cast<CL_Parse_crosshairangle_orig>((DWORD)base_address + (DWORD)0x1914570);

typedef int (*CL_Parse_soundfade_orig)();
static CL_Parse_soundfade_orig CL_Parse_soundfade = reinterpret_cast<CL_Parse_soundfade_orig>((DWORD)base_address + (DWORD)0x1914930);

typedef int (*CL_Parse_filetxferfailed_orig)();
static CL_Parse_filetxferfailed_orig CL_Parse_filetxferfailed = reinterpret_cast<CL_Parse_filetxferfailed_orig>((DWORD)base_address + (DWORD)0x19152E0);

typedef void (*CL_Parse_hltv_orig)();
static CL_Parse_hltv_orig CL_Parse_hltv = reinterpret_cast<CL_Parse_hltv_orig>((DWORD)base_address + (DWORD)0x1914150);

typedef int (*CL_Parse_director_orig)();
static CL_Parse_director_orig CL_Parse_director = reinterpret_cast<CL_Parse_director_orig>((DWORD)base_address + (DWORD)0x1915040);

typedef int (*CL_Parse_voiceinit_orig)();
static CL_Parse_voiceinit_orig CL_Parse_voiceinit = reinterpret_cast<CL_Parse_voiceinit_orig>((DWORD)base_address + (DWORD)0x19150B0);

typedef void (*CL_Parse_voicedata_orig)();
static CL_Parse_voicedata_orig CL_Parse_voicedata = reinterpret_cast<CL_Parse_voicedata_orig>((DWORD)base_address + (DWORD)0x19150D0);

typedef int (*CL_Parse_sendextrainfo_orig)();
static CL_Parse_sendextrainfo_orig CL_Parse_sendextrainfo = reinterpret_cast<CL_Parse_sendextrainfo_orig>((DWORD)base_address + (DWORD)0x1914470);

typedef void (*CL_Parse_timescale_orig)();
static CL_Parse_timescale_orig CL_Parse_timescale = reinterpret_cast<CL_Parse_timescale_orig>((DWORD)base_address + (DWORD)0x1915070);//55

#endif // hooked_client_h__
