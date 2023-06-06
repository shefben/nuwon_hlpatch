/*
 * hlds 3.1.1.0 patch against
 *
 * buffer-overflow vulnerability in info-string handling
 *   and
 * wrong handling of malformed connection request to non-won servers
 *
 * by Virtual Master
 *
 */
#include <stdio.h>
#define __USE_GNU
#include <dlfcn.h>

#include <netinet/in.h>
#include <linux/net.h> 

typedef enum
{
	at_notice,
	at_console,	// same as at_notice, but forces a ConPrintf, not a message box
	at_aiconsole,	// same as at_console, but only shown if developer level is 2!
	at_warning,
	at_error,
	at_logged	// Server print to console ( only in multiplayer games ).
} ALERT_TYPE;

/* handle to engine shared object */
void* handle = NULL;

/* remeber that we already told the user about boffix loading */
int firsttime = 0;

/* function-pointers for engine-functions */
typedef void(*torigSV_ConnectClient)(void);
torigSV_ConnectClient origSV_ConnectClient = NULL;
typedef int(*torigCmd_Argc)(void);
torigCmd_Argc origCmd_Argc = NULL;
typedef char*(*torigCmd_Argv)(int);
torigCmd_Argv origCmd_Argv = NULL;
typedef void(*torigSV_RejectConnection)(unsigned int[5], char*, unsigned int);
torigSV_RejectConnection origSV_RejectConnection = NULL;
typedef void(*torigServerPrint)(const char*);
torigServerPrint origServerPrint = NULL;
typedef void(*torigCon_Init)(void);
torigCon_Init origCon_Init = NULL;
typedef void(*torigAlertMessage)(ALERT_TYPE, char *szFmt, ...);
torigAlertMessage origAlertMessage = NULL;
typedef char(*toriginfo_ValueForKey)(const char, const char);
toriginfo_ValueForKey orig_ValueForKey = NULL;
typedef void(*torigInfo_SetValueForKey)(char, const char, const char, int);
torigInfo_SetValueForKey orig_SetValueForKey = NULL;
typedef void(*toriginfo_RemoveKey)(char, const char);
toriginfo_RemoveKey orig_RemoveKey = NULL;

/* check if info-string is valid */
int isValid(char *info)
{

	char *param, *value;
	int itemsize;
	
	/* valid info-strings may begin with a '\', which needs to be stripped */
	param = info;
	if (param[0] == '\\')
		param++;

	/* the string must not terminate before we got a param\value pair */
	if (param[0] == '\0')
		return 0;

	do
	{
		/* we check parameter size, to make sure it doesn't overflow buffer later*/
		itemsize = 0;
		do {
			/* there can't be a \0 or a parameter name > 127 */
			if ((*param == '\0') || (itemsize > 127))
				return 0;
			itemsize++;
			param++;
		/* carry on when serparating \ is found */
		} while (*param != '\\');

		param++;
		/* value can't begin with \0, and must not be zero-length (no delimiter after a delimiter) */
		if ((*param == '\0') || (*param == '\\'))
			return 0;

		/* carry on with param */
		value = param;
		itemsize = 0;
		do {
			/* there can't be a parameter value > 127 */
			if (itemsize > 127)
				return 0;
			itemsize++;
			value++;
		/* carry on when delimiter is found or input-string is terminated */
		} while ((*value != '\0') && (*value != '\\'));

		/* if there's a delimiter, jump over it */
		if (*value == '\\')
			value++;
		param = value;

	/* carry on as long there's no terminating zero as next character */
	} while ((*value != '\0') && (*(value++) != '\0'));
	
	return 1;
	
}

void getFnPtrs(void)
{
	char *error;
	
	if (!handle)
		handle = dlopen("./engine_i386.so", RTLD_NOW);
	if (!origSV_ConnectClient)
		origSV_ConnectClient = (torigSV_ConnectClient) dlsym (handle, "SV_ConnectClient");
	if (!origCmd_Argc)
		origCmd_Argc = (torigCmd_Argc) dlsym (handle, "Cmd_Argc");
	if (!origCmd_Argv)
		origCmd_Argv = (torigCmd_Argv) dlsym (handle, "Cmd_Argv");
	if (!origSV_RejectConnection)
		origSV_RejectConnection = (torigSV_RejectConnection) dlsym (handle, "SV_RejectConnection");
	if (!origServerPrint)
		origServerPrint = (torigServerPrint) dlsym (handle, "ServerPrint");
	if (!origCon_Init)
		origCon_Init = (torigCon_Init) dlsym (handle, "Con_Init");
	if (!origAlertMessage)
		origAlertMessage = (torigAlertMessage) dlsym (handle, "AlertMessage");
	if ( !orig_ValueForKey )
		orig_ValueForKey = (toriginfo_ValueForKey) dlsym (handle, "Info_ValueForKey");
	if ( !orig_SetValueForKey )
		orig_SetValueForKey = (torigInfo_SetValueForKey) dlsym (handle, "Info_SetValueForKey");
	if ( !orig_RemoveKey )
		orig_RemoveKey = (toriginfo_RemoveKey) dlsym (handle, "Info_RemoveKey");

	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(1);
	}
}

void Con_Init(void)
{
	getFnPtrs();

        if (firsttime == 0)
        {
		/* Tell user about successfull hooking */
		origServerPrint("boffix_i386.so - v3 - successfully loaded and hooks installed\n");
		firsttime = 1;
	}


}

void SV_ConnectClient(void)
{
	int argc = 0;
	char* argv[128];

	unsigned int netfrom[5], *pnetfrom;

	int i=0;

	getFnPtrs();
	pnetfrom = (unsigned int*) dlsym (handle, "net_from");
	
	// copy net_from structure required for SV_RejectConnection
	for (i=5; i>0; i--)
	{
		netfrom[5-i] = *(pnetfrom++);
	}

	argc = origCmd_Argc();
	/* Left for debug purposes...
	printf("netfrom: 0x%x 0x%x 0x%x 0x%x 0x%x\n", netfrom[0], netfrom[1], netfrom[2], netfrom[3], netfrom[4]);
	printf("Argument count: %d\n", argc); */
	
	argc = (argc < 128) ? argc : 127;
	
	for (i=1; i<argc; i++) {
		argv[i] = origCmd_Argv(i);
		/* debug only ...
		 * printf("Argv[%d]: %s\n", i, argv[i]); */
	}
	fflush(stdout);


	if (isValid(argv[3]) && isValid(argv[4])) {
		// argv[3] contains info buffer!!
		//if prot != 2 (won protocol) won only reject
		//if raw == "00000000000000000000000000000000" attach detected
		//if unique == *hltv reject no hltv servers allowed
		//if name == "" reject tell to set name
		//if all is OK then:
		/*
		Copy userinfo to char with null terminator
		sprintf(userinfo, "%s\n", originfo);
		Do checksum of User information
		cksum(uid, userinfo);
		remove all unneeded keys
		remove key "s0 - s9"
		remove key "bottomcolor"
		remove key "topcolor"
		remove key " model"
		set unique to the checksum of playerinfo
		Info_SetValueForKey (cl->userinfo, "unique", uid, MAX_INFO_STRING);
		send to original sv_connectclient :) and you are done 
		*/
/*!!!! Removekeys from argv[4] !!!!*/
		char* Userbuff;
		char* UserID;
		Userbuff = argv[3];
		Userbuff + '\n';// Add Null terminator to UserBuff
        cksum(&UserID, Userbuff); // Use CheckSum to create new WonID

		orig_SetValueForKey(Userbuff, "unique", UserID, 256);
		origSV_ConnectClient();
	}
	else {
		/* Tell console about reject... might be removed later */
		origServerPrint("Invalid userinfo in connect command, dropping connection attempt\n");
		/* Log address */
		origAlertMessage(at_logged, "boffix: Possible overflow attack from %s\n",  inet_ntoa(netfrom[1]));
		/* Reject that connection */
		origSV_RejectConnection(netfrom, "Invalid userinfo in connect command", 0x3ff);
	}

}
