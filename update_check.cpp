
/****************************************************************************
 * The starting point for all WON enabled products is the creation of the	*
 * WONAPICore object. This object also creates the required message threads.*
 ****************************************************************************/


/* Before you begin...
		- Include the path for, and link to, the WONAPI.lib. (../../debug)
		- Include the path for the WONAPI header files. (../../)
		- Be sure you are linking to the Multithread version run time libraries.
		- Link to either wsock32.lib or ws2_32.lib.
		- If you are going to be using platform specific code be sure to 
			include	WONCommon/Platform.h.
*/

#include "WONCommon/Platform.h"					// Platform specfic headers
#include "WONApi.h"								// API namespace. (WONAPICore defined)
#include "WONDir/GetDirOp.h"					// Get Directory Operation
#include "WONCommon/SmartPtr.h"					// Threadsafe Auto-deleting pointers (GetMOTDOpPtr)
#include "WONServer/ServerContext.h"			// Class for communicating with a Titan server
#include "WONAuth/AuthContext.h"				// ServerContext for the auth servers
#include "WONMisc/GetVersionOp.h"
#include "Defines.h"

using namespace WONAPI;

// STATICS (server lists as globals for simplicity)
WONAPICoreEx	 gAPICore;						// Required by WONAPI
ServerContextPtr gDirContext;					// Used to speak to the dir server
ServerContextPtr gGameServerContext;			//		"	"	 to the game servers
AuthContextPtr	 gAuthContext;					// Contains an auth context. (Used similar to those above)
DirEntityList	 gDirectoryContents;			// Stores the contents of the directory
const wchar_t*	DIR_GAME			=	L"/Half-life";				// Game directory. (/GameName)
const wchar_t*	DIR_TITANSERVERS	=	L"/TitanServers/Auth";		// Directory for Auth/Firewall/Contest servers
const wchar_t*	AUTH_SERVER			=	L"AuthServer";				// Authenticates user information
const wchar_t*	GAME_SERVER			=	L"Version";	// Sample game is a basic routing server.

static bool hlcheck = 0;
// PROTOTYPES
bool LoadDirServerAddrs();					// Loads the DirServer addrs into the context
bool GetTitanServices();					// Retrieves the Auth and Firewall addrs
bool GetGameServers();						// Retrieves the Factory and Game servers for DIR_GAME


//******************************************************************************
//  Entry point
//******************************************************************************
int check_version()
{
	// The WON Directory Server acts much like a yellow pages, providing a categorized listings of 
	//	available services (servers). It maintains a hierarchical structure of directory entities, 
	//	much like a file system. Services may be added in any existing directory. A given directory 
	//	may contain any number of services and any number of subdirectories. Every service and 
	//	subdirectory must be unique within a given directory. 
	//
	//	(See api documentation for full desciption)
	gAPICore.SetDoPumpThread(true);
	gAPICore.Startup();
	gDirContext			= new ServerContext();			// Auto-deleting smart pointer (see above for var desc.)
	gGameServerContext	= new ServerContext();			// Auto-deleting smart pointer (see above for var desc.)
	gAuthContext		= new AuthContext();			// Auto-deleting smart pointer (see above for var desc.)

	// The first thing to do is to obtain the IP addresses of the dir servers..
	// DirServer IP addresses should be loaded from an outside, modifiable source. (such as a config file)
	if (!LoadDirServerAddrs())
	{
//		MessageBox(NULL, "LoadDirServerAddrs", "LoadDirServerAddrs", MB_ICONEXCLAMATION);
		return -1;//error
	}
	// Now that the IP of a dir server is known, it can be quereyed for 
	// other services using a GetDirOp
	if (!GetTitanServices())
	{
//		MessageBox(NULL, "GetTitanServices", "GetTitanServices", MB_ICONEXCLAMATION);
		return -1;//error
	}

	// Here is an example of retrieving a game directory
	if (!GetGameServers())
	{
	//	MessageBox(NULL, "GetGameServers", "GetGameServers", MB_ICONEXCLAMATION);
		return -1;//error
	}

	// Shutdown the APICore
	gAPICore.Shutdown();

	// Success
	return hlcheck;

}


//******************************************************************************
// Loads the directory server addresses
//******************************************************************************
bool LoadDirServerAddrs()
{
	// I'm just hardcoding them in for simplicity
	gDirContext->AddAddress( IPAddr("dir1.nuwon.net",15101) );
	gDirContext->AddAddress( IPAddr("dir2.nuwon.net",15101) );
	gDirContext->AddAddress( IPAddr("dir3.nuwon.net",15101) );

	return true;
}

//******************************************************************************
// Retrieves the Contents of the game directory. In the case if this sample,
// it will often be empty.
//******************************************************************************
bool GetGameServers()
{
	GetVersionOpPtr l_pGetVerOp = new GetVersionOp(gDirContext, "Half-Life");
	//l_pGetVerOp->SetCompletion(new WindowCompletion(OPTYPE_GETVER));
	l_pGetVerOp->RunBlock(OP_TIMEOUT_INFINITE);
	if (! l_pGetVerOp->Succeeded())
		return false;
	GetVersionOp *anOp = (GetVersionOp*)l_pGetVerOp;

// 	hlcheck = anOp->VersionIsLatest("1.1.0.0");
// 	return true;
	switch (anOp->CheckVersion("1.1.1.4"))
	{
	case Version_Invalid:
	hlcheck = 0;
		return true;
		break;
	case Version_Valid_NotLatest:
		hlcheck = 0;
		return true;
		break;
	case Version_Valid_Latest:
		hlcheck = 1;
		return true;
		break;
	}

	return true;
}


//******************************************************************************
// Retrieves the available Authentication servers
//******************************************************************************
bool GetTitanServices()
{
	// Construct the Get Directory Operation
	GetDirOpPtr theGetDirOp = new GetDirOp(gDirContext);// Auto deleting smart pointer
	theGetDirOp->SetPath( DIR_TITANSERVERS );			// Directory for other titan servers
	theGetDirOp->SetFlags(  DIR_GF_DECOMPRECURSIVE |	// Parse subdirectories
							DIR_GF_DECOMPSERVICES |		// Include services
							DIR_GF_SERVADDNAME |		// Include service names
							DIR_GF_ADDDISPLAYNAME |		// Include display names
							DIR_GF_SERVADDNETADDR );	// Include IP addr

	// Run the constructed OP synchronously (wait until finished)
	WONStatus result = theGetDirOp->Run(OP_MODE_BLOCK /*sync*/, OP_TIMEOUT_INFINITE);
	if (result != WS_Success)
	{
		printf("Could not retrieve contents of %S.\r\n", DIR_TITANSERVERS);
		return false;/*error*/
	}
	else
		printf("Retrieved contents of %S.\r\n", DIR_TITANSERVERS);
	
	// Gain access to the retrieved directory listing
	gDirectoryContents = theGetDirOp->GetDirEntityList();

	// Parse through the list for auth servers (don't need contest or firewall)
	gAuthContext->GetServerContext()->AddAddressesFromDir(gDirectoryContents,AUTH_SERVER);

	// Display the list of auth servers and some useful info
	DirEntityList::const_iterator aDirEntityItr = gDirectoryContents.begin();
	for (; aDirEntityItr != gDirectoryContents.end(); ++aDirEntityItr )
		printf("\t%s\r\n", (*aDirEntityItr)->GetNetAddrAsIP().GetHostAndPortString().c_str() );

	return true;
}

