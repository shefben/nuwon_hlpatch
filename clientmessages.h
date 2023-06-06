#ifndef clientmessages_h__
#define clientmessages_h__

/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
** Date: 2015/05/01 22:22
** File:clientmessages.h
** Description:  Contains the Message Parse struct (which contains functionpointer)
**
** Author: Benjamin Shefte
** TODO:
**
** -------------------------------------------------------------------------*/

// contains all of the svc messages for client parsing

#include "hooked_funcs.h"
#include "msgdefs.h"
#include "hooked_client.h"

typedef struct svc_func_s
{
	unsigned char opcode;			// Opcode
	char *pszname;					// Display Name
	void ( *pfnParse )( void );		// Parse function
} svc_func_t;

// Dispatch table
static svc_func_t cl_parsefuncs[] =
{
	{ svc_bad, "svc_bad", NULL },																// 0
	{ svc_nop, "svc_nop", NULL },																// 1
	{ svc_disconnect, "svc_disconnect",(void (*) (void))CL_Parse_disconnect},					// 2
	{ svc_event, "svc_event",(void (*) (void))CL_Parse_event},								    // 3
	{ svc_version, "svc_version", (void (*) (void))CL_Parse_version },  						// 4
	{ svc_setview, "svc_setview", (void (*) (void))CL_Parse_setview},  							// 5
	{ svc_sound, "svc_sound", CL_Parse_sound },													// 6
	{ svc_time, "svc_time", CL_Parse_time },													// 7
	{ svc_print, "svc_print", CL_Parse_print },													// 8
	{ svc_stufftext, "svc_stufftext", CL_Parse_stufftext },										// 9
	{ svc_setangle, "svc_setangle", (void (*) (void))CL_Parse_setangle },						// 10
	{ svc_serverinfo, "svc_serverinfo", CL_Parse_serverinfo},									// 11
	{ svc_lightstyle, "svc_lightstyle", (void (*) (void))CL_Parse_lightstyle},					// 12
	{ svc_updateuserinfo, "svc_updateuserinfo", (void (*) (void))CL_Parse_updateuserinfo },		// 13
	{ svc_deltadescription, "svc_deltadescription", (void (*) (void))CL_Parse_deltadescription }, // 14
	{ svc_clientdata, "svc_clientdata", (void (*) (void))CL_Parse_clientdata },					// 15
	{ svc_stopsound, "svc_stopsound", (void (*) (void))CL_Parse_stopsound },					// 16
	{ svc_pings, "svc_pings", (void (*) (void))CL_Parse_pings },								// 17
	{ svc_particle, "svc_particle", (void (*) (void))CL_Parse_particle },						// 18
	{ svc_damage, "svc_damage", CL_Parse_damage },												// 19
	{ svc_spawnstatic, "svc_spawnstatic", (void (*) (void))CL_Parse_spawnstaticsound },			// 20
	{ svc_event_reliable, "svc_event_reliable", (void (*) (void))CL_Parse_eventreliable },		// 21
	{ svc_spawnbaseline, "svc_spawnbaseline", (void (*) (void))CL_Parse_spawnbaseline },		// 22
	{ svc_tempentity, "svc_tempentity", CL_Parse_tempentity },									// 23
	{ svc_setpause, "svc_setpause", (void (*) (void))CL_Parse_setpause },						// 24
	{ svc_signonnum, "svc_signonnum", (void (*) (void))CL_Parse_signonnum },					// 25
	{ svc_centerprint, "svc_centerprint", (void (*) (void))CL_Parse_centerprint },				// 26
	{ svc_killedmonster, "svc_killedmonster", CL_Parse_killedmonster },							// 27
	{ svc_foundsecret, "svc_foundsecret", CL_Parse_foundsecret },								// 28
	{ svc_spawnstaticsound, "svc_spawnstaticsound", (void (*) (void))CL_Parse_spawnstaticsound }, // 29
	{ svc_intermission, "svc_intermission",(void (*) (void)) CL_Parse_intermission},			// 30
	{ svc_finale, "svc_finale", (void (*) (void))CL_Parse_finale },								// 31
	{ svc_cdtrack, "svc_cdtrack", (void (*) (void))CL_Parse_cdtrack },							// 32
	{ svc_restore, "svc_restore", (void (*) (void))CL_Parse_restore },							// 33
	{ svc_cutscene, "svc_cutscene", (void (*) (void))CL_Parse_cutscene },						// 34
	{ svc_weaponanim, "svc_weaponanim", (void (*) (void))CL_Parse_weaponanim},					// 35
	{ svc_decalname, "svc_decalname", (void (*) (void))CL_Parse_decalname },					// 36
	{ svc_roomtype, "svc_roomtype", CL_Parse_roomtype },										// 37
	{ svc_addangle, "svc_addangle", CL_Parse_addangle },										// 38
	{ svc_newusermsg, "svc_newusermsg", (void (*) (void))CL_Parse_newusermsg},					// 39
	{ svc_packetentities, "svc_packetentities", (void (*) (void))CL_Parse_packetentities },		// 40
	{ svc_deltapacketentities, "svc_deltapacketentities", (void (*) (void))CL_Parse_deltapacketentities},  // 41
	{ svc_choke, "svc_choke", (void (*) (void))CL_Parse_choke },								// 42
	{ svc_resourcelist, "svc_resourcelist", CL_Parse_resourcelist },							// 43
	{ svc_newmovevars, "svc_newmovevars", (void (*) (void))CL_Parse_newmovevars },				// 44
	{ svc_resourcerequest, "svc_resourcerequest", (void (*) (void))CL_Parse_resourcerequest },  // 45
	{ svc_customization, "svc_customization", CL_Parse_customization },							// 46
	{ svc_crosshairangle, "svc_crosshairangle", (void (*) (void))CL_Parse_crosshairangle },	    // 47
	{ svc_soundfade, "svc_soundfade",	(void (*) (void))CL_Parse_soundfade },					// 48
	{ svc_filetxferfailed, "svc_filetxferfailed", (void (*) (void))CL_Parse_filetxferfailed	 }, // 49
	{ svc_hltv, "svc_hltv",	CL_Parse_hltv },													// 50
	{ svc_director, "svc_director", (void (*) (void))CL_Parse_director },						// 51
	{ svc_voiceinit, "svc_voiceinit",	(void (*) (void))CL_Parse_voiceinit },					// 52
	{ svc_voicedata , "svc_voicedata", CL_Parse_voicedata	 },									// 53
	{ svc_sendextrainfo, "svc_sendextrainfo",	(void (*) (void))CL_Parse_sendextrainfo },		// 54
	{ svc_timescale, "svc_timescale", CL_Parse_timescale },										// 55 
	// Here are the special 'steam' messages that we needed to add
	{ svc_resourcelocation, "svc_resourcelocation",	CL_ParseResourceLocation },					// 56
	{ svc_sendcvarvalue, "svc_sendcvarvalue", CL_Send_CvarValue },								// 57
	{ svc_sendcvarvalue2, "svc_sendcvarvalue2", CL_Send_CvarValue2 },							// 58
	{ (unsigned char)-1, "End of List", NULL }
};
#endif // clientmessages_h__
