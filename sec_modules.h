

extern void LoadSecurityModuleFromDisk(char * pszDllName);
extern void LoadSecurityModuleFromMemory( unsigned char * pCode, int nSize );
extern void CloseSecurityModule();
// Module exports
extern modfuncs_t g_modfuncs;
extern module_t	g_module;

typedef struct module_s
{
	unsigned char				ucMD5Hash[16];	// hash over code
	qboolean					fLoaded;		// true if successfully loaded
} module_t;


// ********************************************************
// Functions exposed by the engine to the module
// ********************************************************

// Functions for ModuleS
typedef void (*PFN_KICKPLAYER)(int nPlayerSlot, int nReason);

typedef struct modshelpers_s
{
	PFN_KICKPLAYER m_pfnKickPlayer;
	
	// reserved for future expansion
	int m_nVoid1;
	int m_nVoid2;
	int m_nVoid3;
	int m_nVoid4;
	int m_nVoid5;
	int m_nVoid6;
	int m_nVoid7;
	int m_nVoid8;
	int m_nVoid9;
} modshelpers_t;

// Functions for moduleC
typedef struct modchelpers_s
{
	// reserved for future expansion
	int m_nVoid0;
	int m_nVoid1;
	int m_nVoid2;
	int m_nVoid3;
	int m_nVoid4;
	int m_nVoid5;
	int m_nVoid6;
	int m_nVoid7;
	int m_nVoid8;
	int m_nVoid9;
} modchelpers_t;



// ********************************************************
// Functions exposed by the security module
// ********************************************************
typedef void (*PFN_LOADMOD)(char *pchModule);
typedef void (*PFN_CLOSEMOD)(void);
typedef int (*PFN_NCALL)(int ijump, int cnArg, ...);

typedef void (*PFN_GETCLDSTADDRS)(cldll_func_dst_t *pcldstAddrs);
typedef void (*PFN_GETENGDSTADDRS)(cl_enginefunc_dst_t *pengdstAddrs);
typedef void (*PFN_MODULELOADED)(void);

typedef void (*PFN_PROCESSOUTGOINGNET)(struct netchan_s *pchan, struct sizebuf_s *psizebuf);
typedef qboolean (*PFN_PROCESSINCOMINGNET)(struct netchan_s *pchan, struct sizebuf_s *psizebuf);

typedef void (*PFN_TEXTURELOAD)(char *pszName, int dxWidth, int dyHeight, char *pbData);
typedef void (*PFN_MODELLOAD)(struct model_s *pmodel, void *pvBuf);

typedef void (*PFN_FRAMEBEGIN)(void);
typedef void (*PFN_FRAMERENDER1)(void);
typedef void (*PFN_FRAMERENDER2)(void);

typedef void (*PFN_SETMODSHELPERS)(modshelpers_t *pmodshelpers);
typedef void (*PFN_SETMODCHELPERS)(modchelpers_t *pmodchelpers);
typedef void (*PFN_SETENGDATA)(engdata_t *pengdata);

typedef void (*PFN_CONNECTCLIENT)(int iPlayer);
typedef void (*PFN_RECORDIP)(unsigned int pnIP);
typedef void (*PFN_PLAYERSTATUS)(unsigned char *pbData, int cbData);

typedef void (*PFN_SETENGINEVERSION)(int nVersion);

// typedef class CMachine *(*PFN_PCMACHINE)(void);
typedef int (*PFN_PCMACHINE)(void);
typedef void (*PFN_SETIP)(int ijump);
typedef void (*PFN_EXECUTE)(void);

typedef struct modfuncs_s
{
	// Functions for the pcode interpreter
	PFN_LOADMOD m_pfnLoadMod;
	PFN_CLOSEMOD m_pfnCloseMod;
	PFN_NCALL m_pfnNCall;
	
	// API destination functions
	PFN_GETCLDSTADDRS m_pfnGetClDstAddrs;
	PFN_GETENGDSTADDRS m_pfnGetEngDstAddrs;
	
	// Miscellaneous functions
	PFN_MODULELOADED m_pfnModuleLoaded;     // Called right after the module is loaded
	
	// Functions for processing network traffic
	PFN_PROCESSOUTGOINGNET m_pfnProcessOutgoingNet;   // Every outgoing packet gets run through this
	PFN_PROCESSINCOMINGNET m_pfnProcessIncomingNet;   // Every incoming packet gets run through this
	
	// Resource functions
	PFN_TEXTURELOAD m_pfnTextureLoad;     // Called as each texture is loaded
	PFN_MODELLOAD m_pfnModelLoad;         // Called as each model is loaded
	
	// Functions called every frame
	PFN_FRAMEBEGIN m_pfnFrameBegin;       // Called at the beginning of each frame cycle
	PFN_FRAMERENDER1 m_pfnFrameRender1;   // Called at the beginning of the render loop
	PFN_FRAMERENDER2 m_pfnFrameRender2;   // Called at the end of the render loop
	
	// Module helper transfer
	PFN_SETMODSHELPERS m_pfnSetModSHelpers;
	PFN_SETMODCHELPERS m_pfnSetModCHelpers;
	PFN_SETENGDATA m_pfnSetEngData;
	
	// Which version of the module is this?
	int m_nVersion;
	
	// Miscellaneous game stuff
	PFN_CONNECTCLIENT m_pfnConnectClient;	// Called whenever a new client connects
	PFN_RECORDIP m_pfnRecordIP;				// Secure master has reported a new IP for us
	PFN_PLAYERSTATUS m_pfnPlayerStatus;		// Called whenever we receive a PlayerStatus packet
	
	// Recent additions
	PFN_SETENGINEVERSION m_pfnSetEngineVersion;	// 1 = patched engine
	
	// reserved for future expansion
	int m_nVoid2;
	int m_nVoid3;
	int m_nVoid4;
	int m_nVoid5;
	int m_nVoid6;
	int m_nVoid7;
	int m_nVoid8;
	int m_nVoid9;
} modfuncs_t;
