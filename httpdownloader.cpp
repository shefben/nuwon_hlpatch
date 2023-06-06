//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
//--------------------------------------------------------------------------------------------------------------
// download.cpp
// 
// Implementation file for optional HTTP asset downloading
// Author: Matthew D. Campbell (matt@turtlerockstudios.com), 2004
//--------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------------------------------


// fopen is needed for the bzip code
// #undef fopen

// #include <WinInet.h>

#include "httpdownloader.h"
#include "utils.h"
#include "hooked_client.h"
#include "hooked_funcs.h"
#include "Defines.h"
#include <process.h>

// memdbgon must be the last include file in a .cpp file!!!
//#include "tier0/memdbgon.h"

static const char *CacheDirectory = "cache";
static const char *CacheFilename = "cache/DownloadCache.db";
Color DownloadColor			(   0, 200, 100, 255 );
Color DownloadErrorColor	( 200, 100, 100, 255 );
Color DownloadCompleteColor	( 100, 200, 100, 255 );

//--------------------------------------------------------------------------------------------------------------
static char * CloneString( const char *original )
{
	char *newString = new char[ Q_strlen( original ) + 1 ];
	Q_strcpy( newString, original );

	return newString;
}

//--------------------------------------------------------------------------------------------------------------
// Class Definitions
//--------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------
DownloadCache::DownloadCache()
{
	m_cache = NULL;
}

//--------------------------------------------------------------------------------------------------------------
DownloadCache::~DownloadCache()
{
}

//--------------------------------------------------------------------------------------------------------------
void DownloadCache::BuildKeyNames( const char *gamePath )
{
	if ( !gamePath )
	{
		m_cachefileKey[0] = 0;
		m_timestampKey[0] = 0;
		return;
	}

	char *tmpGamePath = CloneString( gamePath );
	char *tmp = tmpGamePath;
	while ( *tmp )
	{
		if ( *tmp == '/' || *tmp == '\\' )
		{
			*tmp = '_';
		}
		++tmp;
	}
	_snprintf( m_cachefileKey, sizeof( m_cachefileKey ), "cachefile_%s", tmpGamePath );
	_snprintf( m_timestampKey, sizeof( m_timestampKey ), "timestamp_%s", tmpGamePath );

	delete[] tmpGamePath;
}

//--------------------------------------------------------------------------------------------------------------
void DownloadCache::Init()
{
	if ( m_cache )
	{
		m_cache->deleteThis();
	}

	m_cache = new KeyValues( "DownloadCache" );
	m_cache->LoadFromFile(CacheFilename);
//	g_pFileSystem->CreateDirHierarchy( CacheDirectory, "DEFAULT_WRITE_PATH" );
}

//--------------------------------------------------------------------------------------------------------------
void DownloadCache::GetCachedData( RequestContext *rc )
{
	FILE *cachedFile;
	if ( !m_cache )
		return;

	char cachePath[_MAX_PATH];
	GetCacheFilename( rc, cachePath );

	if ( !(*cachePath) )
		return;

	cachedFile = fopen(cachePath, "rb");

	fseek (cachedFile, 0, SEEK_END);
	int size = ftell(cachedFile);

	rc->cacheData = new unsigned char[size];

	int status = fread(rc->cacheData, sizeof(char),size, cachedFile);
	fclose(cachedFile);

	if ( !status )
	{
		delete[] rc->cacheData;
		rc->cacheData = NULL;
	}
	else
	{
		BuildKeyNames( rc->gamePath );
		rc->nBytesCached = size;
		strncpy( rc->cachedTimestamp, m_cache->GetString( m_timestampKey, "" ), BufferSize );
	}
}

//--------------------------------------------------------------------------------------------------------------
/**
 *  Takes a data stream compressed with bzip2, and writes it out to disk, uncompresses it, and deletes the
 *  compressed version.
 */
static bool DecompressBZipToDisk( const char *outFilename, const char *srcFilename, char *data, int bytesTotal )
{
	if ( FS_FileExists( (int) outFilename ) != -1 || !data || bytesTotal < 1 )
	{
		return false;
	}

	// Create the subdirs
	char * tmpDir = CloneString( outFilename );
	COM_CreatePath( (char)tmpDir );
	delete[] tmpDir;
	FILE* CachedFile;
	// open the file for writing
	char fullSrcPath[MAX_PATH];
	Q_MakeAbsolutePath( fullSrcPath, sizeof( fullSrcPath ), srcFilename, (const char*)com_gamedir );

	if ( FS_FileExists( (int) fullSrcPath ) != -1 )
	{
		// Write out the .bz2 file, for simplest decompression
		CachedFile  = fopen( fullSrcPath, "wb" );
		if ( !CachedFile )
		{
			return false;
		}
		int bytesWritten = fwrite( data,sizeof(char), bytesTotal, CachedFile );
		fclose( CachedFile );
		if ( bytesWritten != bytesTotal )
		{
			// couldn't write out all of the .bz2 file
			remove( srcFilename );
			return false;
		}
	}

	// Prepare the uncompressed filehandle
	CachedFile  = fopen( fullSrcPath, "wb" );
	if ( !CachedFile )
	{
		remove( srcFilename );
		return false;
	}

	// And decompress!
	const int OutBufSize = 65536;
	char    buf[ OutBufSize ];
	BZFILE *bzfp = BZ2_bzopen( fullSrcPath, "rb" );
	while ( 1 )
	{
		int bytesRead = BZ2_bzread( bzfp, buf, OutBufSize );
		if ( bytesRead < 0 )
		{
			break; // error out
		}

		if ( bytesRead > 0 )
		{
			int bytesWritten = fwrite( buf, sizeof(unsigned char), bytesRead,CachedFile  );
			if ( bytesWritten != bytesRead )
			{
				break; // error out
			}
		}
		else
		{
			fclose( CachedFile );
			BZ2_bzclose( bzfp );
			remove( srcFilename );
			return true;
		}
	}

	// We failed somewhere, so clean up and exit
	fclose( CachedFile );
	BZ2_bzclose( bzfp );
	remove( srcFilename );
	remove( outFilename );
	return false;
}

//--------------------------------------------------------------------------------------------------------------
void DownloadCache::PersistToDisk( const RequestContext *rc )
{
	FILE* CachedFile;
	if ( !m_cache )
		return;

	if ( rc && rc->data && rc->nBytesTotal )
	{
		char gamePath[MAX_PATH];
		if ( rc->bIsBZ2 )
		{
			Q_StripExtension( rc->gamePath, gamePath, sizeof( gamePath ) );
		}
		else
		{
			Q_strncpy( gamePath, rc->gamePath, sizeof( gamePath ) );
		}

		if ( FS_FileExists( (int) gamePath ) == -1 )
		{
			// Create the subdirs
			char * tmpDir = CloneString( gamePath );
			COM_CreatePath( (char)tmpDir );
			delete[] tmpDir;

			bool success = false;
			if ( rc->bIsBZ2 )
			{
				success = DecompressBZipToDisk( gamePath, rc->gamePath, reinterpret_cast< char * >(rc->data), rc->nBytesTotal );
			}
			else
			{
				CachedFile = fopen( gamePath, "wb" );
				if ( CachedFile )
				{
					fwrite( rc->data,sizeof(unsigned char), rc->nBytesTotal, CachedFile );
					fclose( CachedFile );
					success = true;
				}
			}

			if ( success )
			{
				// write succeeded.  remove any old data from the cache.
				char cachePath[_MAX_PATH];
				GetCacheFilename( rc, cachePath );
				if ( cachePath[0] )
				{
					remove( cachePath );
				}

				BuildKeyNames( rc->gamePath );
				KeyValues *kv = m_cache->FindKey( m_cachefileKey, false );
				if ( kv )
				{
					m_cache->RemoveSubKey( kv );
				}
				kv = m_cache->FindKey( m_timestampKey, false );
				if ( kv )
				{
					m_cache->RemoveSubKey( kv );
				}
			}
		}
	}

	m_cache->SaveToFile(CacheFilename);
}

//--------------------------------------------------------------------------------------------------------------
void DownloadCache::PersistToCache( const RequestContext *rc )
{
	FILE* CachedFile;
	if ( !m_cache || !rc || !rc->data || !rc->nBytesTotal || !rc->nBytesCurrent )
		return;

	char cachePath[_MAX_PATH];
	GenerateCacheFilename( rc, cachePath );

	CachedFile = fopen(cachePath, "wb" );
	if ( CachedFile )
	{
// 		fwrite(rc->data,sizeof(DWORD),rc->nBytesCurrent,CachedFile);
// 		fclose(CachedFile);

		m_cache->SaveToFile(CacheFilename);
	}
}

//--------------------------------------------------------------------------------------------------------------
void DownloadCache::GetCacheFilename( const RequestContext *rc, char cachePath[_MAX_PATH] )
{
	BuildKeyNames( rc->gamePath );
	const char *path = m_cache->GetString( m_cachefileKey, NULL );
	if ( !path || strncmp( path, CacheDirectory, strlen(CacheDirectory) ) )
	{
		cachePath[0] = 0;
		return;
	}
	strncpy( cachePath, path, _MAX_PATH );
	cachePath[_MAX_PATH-1] = 0;
}

//--------------------------------------------------------------------------------------------------------------
void DownloadCache::GenerateCacheFilename( const RequestContext *rc, char cachePath[_MAX_PATH] )
{
	GetCacheFilename( rc, cachePath );
	BuildKeyNames( rc->gamePath );

	m_cache->SetString( m_timestampKey, rc->cachedTimestamp );
	//Con_DPrintf("DownloadCache::GenerateCacheFilename() set %s = %s\n", m_timestampKey, rc->cachedTimestamp );

	if ( !*cachePath )
	{
		const char * lastSlash = strrchr( rc->gamePath, '/' );
		const char * lastBackslash = strrchr( rc->gamePath, '\\' );
		const char *gameFilename = rc->gamePath;
		if ( lastSlash || lastBackslash )
		{
			gameFilename = max( lastSlash, lastBackslash ) + 1;
		}
		for( int i=0; i<1000; ++i )
		{
			Q_snprintf( cachePath, _MAX_PATH, "%s/%s%4.4d", CacheDirectory, gameFilename, i );
			if ( FS_FileExists( (int) cachePath ) == -1 )
			{
				m_cache->SetString( m_cachefileKey, cachePath );
				//Con_DPrintf("DownloadCache::GenerateCacheFilename() set %s = %s\n", m_cachefileKey, cachePath );
				return;
			}
		}
		// all 1000 were invalid?!?
		Q_snprintf( cachePath, _MAX_PATH, "%s/overflow", CacheDirectory );
		//Con_DPrintf("DownloadCache::GenerateCacheFilename() set %s = %s\n", m_cachefileKey, cachePath );
		m_cache->SetString( m_cachefileKey, cachePath );
	}
}

//////////////////////////////////////////////////////////////////////////
// Download Manager Code Starts Here
//////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------------------
DownloadManager::DownloadManager()
{
	m_activeRequest = NULL;
	m_lastPercent = 0;
	m_totalRequests = 0;
}

//--------------------------------------------------------------------------------------------------------------
DownloadManager::~DownloadManager()
{
	Reset();

	for ( int i=0; i<m_downloadedMaps.Count(); ++i )
	{
		delete[] m_downloadedMaps[i];
	}
	m_downloadedMaps.RemoveAll();
}

//--------------------------------------------------------------------------------------------------------------
bool DownloadManager::HasMapBeenDownloadedFromServer( const char *serverMapName )
{
	if ( !serverMapName )
		return false;

	for ( int i=0; i<m_downloadedMaps.Count(); ++i )
	{
		const char *oldServerMapName = m_downloadedMaps[i];
		if ( oldServerMapName && !stricmp( serverMapName, oldServerMapName ) )
		{
			return true;
		}
	}
	return false;
}

bool DownloadManager::FileDenied( const char *filename, unsigned int requestID )
{
	if ( !m_activeRequest )
		return false;

	if ( m_activeRequest->nRequestID != requestID )
		return false;

	if ( m_activeRequest->bAsHTTP )
		return false;

	Con_Printf( "Error downloading %s\n", m_activeRequest->gamePath );
	UpdateProgressBar();

	// try to download the next file
	m_completedRequests.AddToTail( m_activeRequest );
	m_activeRequest = NULL;

	return true;
}

bool DownloadManager::FileReceived( const char *filename, unsigned int requestID )
{
	if ( !m_activeRequest )
		return false;

	if ( m_activeRequest->nRequestID != requestID )
		return false;

	if ( m_activeRequest->bAsHTTP )
		return false;

	Con_Printf( "Download Complete!\n" );
	UpdateProgressBar();

	m_completedRequests.AddToTail( m_activeRequest );
	m_activeRequest = NULL;

	return true;
}

//--------------------------------------------------------------------------------------------------------------
void DownloadManager::MarkMapAsDownloadedFromServer( const char *serverMapName )
{
	if ( !serverMapName )
		return;

	if ( HasMapBeenDownloadedFromServer( serverMapName ) )
		return;

	m_downloadedMaps.AddToTail( CloneString( serverMapName ) );


	return;
}

//--------------------------------------------------------------------------------------------------------------
void DownloadManager::Queue( const char *baseURL, const char *gamePath )
{
	//Con_Printf("\nqueue dlurl: %s\n",svurl);
	bool bAsHTTP = true;
	if ( !gamePath )
		return;
	if ( sv_active == 1 )
		return;	// don't try to download things for the local server (in case a map is missing sounds etc that
				// aren't needed to play.
/*
	if ( bAsHTTP && !FS_FileExists( (int) va( "%s.bz2", gamePath ) ) )
	{
		// Queue up an HTTP download of the bzipped asset, in case it exists.
		// When a bzipped download finishes, we'll uncompress the file to it's
		// original destination, and the queued download of the uncompressed
		// file will abort.

		++m_totalRequests;
		if ( !TheDownloadCache )
		{
			TheDownloadCache = new DownloadCache;
			TheDownloadCache->Init();
		}

		RequestContext *rc = new RequestContext;
		m_queuedRequests.AddToTail( rc );

		memset( rc, 0, sizeof(RequestContext) );

		rc->status = HTTP_CONNECTING;

		Q_strncpy( rc->basePath, (const char*)com_gamedir, BufferSize );//Path to mod folder (on computer)
		Q_strncpy( rc->gamePath, gamePath, BufferSize );//appends file  path to  url
		Q_strncat( rc->gamePath, ".bz2", BufferSize );
		Q_FixSlashes( rc->gamePath, '/' ); // only matters for debug prints, which are full URLS, so we want forward slashes
		Q_strncpy( rc->serverURL, (const char*)net_from, BufferSize ); // game server ip For reconnect and http referal

		rc->bIsBZ2 = true;
		rc->bAsHTTP = true;
		Q_strncpy( rc->baseURL, baseURL, BufferSize ); //downloadurl website http://
		Q_strncat( rc->baseURL, "/", BufferSize );

		Con_DPrintf( "Queueing %s%s.\n", rc->baseURL, gamePath );
	}
*/
	++m_totalRequests;
	if ( !TheDownloadCache )
	{
		TheDownloadCache = new DownloadCache;
		TheDownloadCache->Init();
	}

	RequestContext *rc = new RequestContext;
	m_queuedRequests.AddToTail( rc );

	memset( rc, 0, sizeof(RequestContext) );

	rc->status = HTTP_CONNECTING;
	 
	Q_strncpy( rc->basePath, (const char*)com_gamedir, BufferSize ); //filesystem mod folder c:/hl/blah
	Q_strncpy( rc->gamePath, gamePath, BufferSize );//url path ie blah.com/gamepath
	Q_FixSlashes( rc->gamePath, '/' ); // only matters for debug prints, which are full URLS, so we want forward slashes
	Q_strncpy( rc->serverURL, va("%s",net_from), BufferSize ); //ip for reconnect

	if ( bAsHTTP )
	{
		rc->bAsHTTP = true;	
		Q_strncpy( rc->baseURL, baseURL, BufferSize );
		Q_strncat( rc->baseURL, "/", BufferSize );
	}
	else
	{
		rc->bAsHTTP = false;
	}
m_totalRequests += 1;
	//Con_DPrintf( "\n Queueing %s%s.\n", rc->baseURL, gamePath );
}

//--------------------------------------------------------------------------------------------------------------
void DownloadManager::Reset()
{
	// ask the active request to bail
	if ( m_activeRequest )
	{
		Con_DPrintf( "Aborting download of %s\n", m_activeRequest->gamePath );
		if ( m_activeRequest->nBytesTotal && m_activeRequest->nBytesCurrent )
		{
			// Persist partial data to cache
			TheDownloadCache->PersistToCache( m_activeRequest );
		}
		m_activeRequest->shouldStop = true;
		m_completedRequests.AddToTail( m_activeRequest );
		m_activeRequest = NULL;
		//TODO: StopLoadingProgressBar();
	}

	// clear out any queued requests
	for ( int i=0; i<m_queuedRequests.Count(); ++i )
	{
		Con_DPrintf( "Discarding queued download of %s\n", m_queuedRequests[i]->gamePath );
		delete m_queuedRequests[i];
	}
	m_queuedRequests.RemoveAll();

	if ( TheDownloadCache )
	{
		delete TheDownloadCache;
		TheDownloadCache = NULL;
	}

	m_lastPercent = 0;
	m_totalRequests = 0;
}

//--------------------------------------------------------------------------------------------------------------
// Check download requests that have been completed to see if their threads have exited
void DownloadManager::PruneCompletedRequests()
{
	for ( int i=m_completedRequests.Count()-1; i>=0; --i )
	{
		if ( m_completedRequests[i]->threadDone || !m_completedRequests[i]->bAsHTTP )
		{
			if ( m_completedRequests[i]->cacheData )
			{
				delete[] m_completedRequests[i]->cacheData;
			}
			delete m_completedRequests[i];
			m_completedRequests.Remove( i );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------
// Checks download status, and updates progress bar
void DownloadManager::CheckActiveDownload()
{
//	Con_DPrintf("\n Checking Active downloads ");
	if ( !m_activeRequest )
		return;

	if ( !m_activeRequest->bAsHTTP )
	{
		return;
	}

	UpdateProgressBar();

	// check active request for completion / error / progress update
	switch ( m_activeRequest->status )
	{
	case HTTP_DONE:
		Con_DPrintf( "Download finished!\n" );
		if ( m_activeRequest->nBytesTotal )
		{
			// Persist complete data to disk, and remove cache entry
			UpdateProgressBar();
			TheDownloadCache->PersistToDisk( m_activeRequest );
			m_activeRequest->shouldStop = true;
			m_completedRequests.AddToTail( m_activeRequest );
			m_activeRequest = NULL;
			if ( !m_queuedRequests.Count() )
			{
				//TODO: StopLoadingProgressBar();
				Cbuf_AddText("retry\n");
			}
		}
		break;
	case HTTP_ERROR:
		UpdateProgressBar();
		Con_Printf( "Error downloading %s%s\n", m_activeRequest->baseURL, m_activeRequest->gamePath );
		// try to download the next file
		m_activeRequest->shouldStop = true;
	//	m_activeRequest->threadDone = true
		m_completedRequests.AddToTail( m_activeRequest );
		m_activeRequest = NULL;
		if ( !m_queuedRequests.Count() )
		{
			;
			//TODO: StopLoadingProgressBar();
			//TODO: Cbuf_AddText("retry\n");
		}
		break;
	case HTTP_FETCH:
		UpdateProgressBar();
		if ( m_activeRequest->nBytesTotal )
		{
			int percent = ( m_activeRequest->nBytesCurrent * 100 / m_activeRequest->nBytesTotal );
			if ( percent != m_lastPercent )
			{
				
				Con_DPrintf( "Downloading %s%s: %3.3d%% - %d of %d bytes\n",
					m_activeRequest->baseURL, m_activeRequest->gamePath,
					percent, m_activeRequest->nBytesCurrent, m_activeRequest->nBytesTotal );
					
				m_lastPercent = percent;
				filepercentage = percent;
			}
			UpdateProgressBar();
		}
		break;
	
	}
}

//--------------------------------------------------------------------------------------------------------------
// Starts a new download if there are queued requests
void DownloadManager::StartNewDownload()
{
	//Con_DPrintf("\n Starting New downloads \n");

	if ( m_activeRequest || !m_queuedRequests.Count() )
	{
		return;
	}
	while ( !m_activeRequest && m_queuedRequests.Count() )
	{
		// Remove one request from the queue and make it active
		m_activeRequest = m_queuedRequests[0];
		m_queuedRequests.Remove( 0 );

		if ( FS_FileExists( (int) m_activeRequest->gamePath ) != -1 )
		{
			Con_DPrintf( "Skipping existing file %s%s.\n", m_activeRequest->baseURL, m_activeRequest->gamePath );
			m_activeRequest->shouldStop = true;
			m_activeRequest->threadDone = true;
			m_completedRequests.AddToTail( m_activeRequest );
			m_activeRequest = NULL;
		}
	}

	if ( !m_activeRequest )
	{
		return;
	}
	if ( FS_FileExists( (int)m_activeRequest->gamePath ) != -1 )
	{
		m_activeRequest->shouldStop = true;
		m_activeRequest->threadDone = true;
		m_completedRequests.AddToTail( m_activeRequest );
		m_activeRequest = NULL;
		return; // don't download existing files
	}

	if ( m_activeRequest->bAsHTTP )
	{
		// Check cache for partial match
		TheDownloadCache->GetCachedData( m_activeRequest );

		//TODO: ContinueLoadingProgressBar( "Http", m_totalRequests - m_queuedRequests.Count(), 0.0f );
		//TODO: SetLoadingProgressBarStatusText( "#GameUI_VerifyingAndDownloading" );
		//TODO: SetSecondaryProgressBarText( m_activeRequest->gamePath );
		//TODO: SetSecondaryProgressBar( 0.0f );
		UpdateProgressBar();

		//Con_DPrintf( "Downloading %s%s.\n", m_activeRequest->baseURL, m_activeRequest->gamePath );
	 //Cbuf_AddText(va("scr_connectmsg \"Downloading %s\"", m_activeRequest->gamePath));

		m_lastPercent = 0;
	//	*(BYTE*)gDownloadFile = (BYTE)m_activeRequest->gamePath;

		// Start the thread
		_beginthreadex(NULL, 0, DownloadThread, m_activeRequest, 0, 0/*&threadID */);

	}
	else
	{
		Con_Printf( "Downloading Map via Fragmentation: %s.\n", m_activeRequest->gamePath );
		if(strcmp(".bsp",m_activeRequest->gamePath) == 1)
		{
			MSG_WriteByte(net_message, 3);
			MSG_WriteString(net_message, (int)va("dlfile %s", m_activeRequest->gamePath));
		}
 		m_lastPercent = 0;
	}
}
//--------------------------------------------------------------------------------------------------------------
void DownloadManager::UpdateProgressBar()
{
	float progress = 0.0f; // Overall progress
	int filePercent = 0;

	if ( !m_activeRequest )
	{
		return;
	} 
	if ( m_activeRequest->bAsHTTP )
	{
		int overallPercent = (m_totalRequests - m_queuedRequests.Count() - 1) * 100 / m_totalRequests;
		if ( m_activeRequest->nBytesTotal > 0 )
		{	
			filepercentage = filePercent = ( m_activeRequest->nBytesCurrent * 100 / m_activeRequest->nBytesTotal );
		}
        progress = (overallPercent + filePercent * 1.0f / m_totalRequests) * 0.01f;
		*(BYTE*)scr_downloading = (BYTE)filepercentage;
	}
	memcpy((DWORD*)gDownloadFile, (DWORD*)m_activeRequest->gamePath, 255);
//	*(BYTE*)gDownloadFile = (unsigned char)va("%s", m_activeRequest->gamePath);
	CL_SetFileDownloadPercent(filepercentage);

	CL_SetTotalDownloadPercent(progress);

}

//--------------------------------------------------------------------------------------------------------------
// Monitors download thread, starts new downloads, and updates progress bar
bool DownloadManager::Update()
{
	PruneCompletedRequests();
	CheckActiveDownload();
	StartNewDownload();

	return m_activeRequest != NULL;
}
//////////////////////////////////////////////////////////////////////////
// Tells us if ALL the downloads are done (including failed downloads)
void DownloadManager::IsThreadFinished()
{
	int totalRequests = m_totalRequests;
	int failedRequests = m_failedRequests;
	int completedRequests = m_completedRequests.Count();

	if(m_totalRequests !=0 && (totalRequests - failedRequests - completedRequests) == 0)
	{//All downloads accounted for, lets retry so we can play
		Con_Command("retry");
	}
}
//--------------------------------------------------------------------------------------------------------------
// Externally-visible function definitions
//--------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------
bool CL_DownloadUpdate(void)
{
	return TheDownloadManager.Update();
}
void CL_UpdateProgressBar()
{
	TheDownloadManager.UpdateProgressBar();
}
void CL_SetURL(const char* url)
{
	TheDownloadManager.SetUrl(url);
}
const char* CL_GetURL(void)
{
	return TheDownloadManager.GetUrl();
}
void CL_IsThreadFinished()
{
	TheDownloadManager.IsThreadFinished();
}
int CL_GetTotalDownloadPercent()
{
	return totalfileprogress;
}
int CL_GetFileDownloadPercent()
{
	return dlprogress;
}
int CL_SetFileDownloadPercent(int percentage)
{
	dlprogress = percentage;
	return dlprogress;
}
int CL_SetTotalDownloadPercent(int percentage)
{
	totalfileprogress = percentage;
	return totalfileprogress;
}
//--------------------------------------------------------------------------------------------------------------
void CL_HTTPStop_f(void)
{
	TheDownloadManager.Stop();
}

bool CL_FileReceived( const char *filename, unsigned int requestID )
{
	return TheDownloadManager.FileReceived( filename, requestID );
}

bool CL_FileDenied( const char *filename, unsigned int requestID )
{
	return TheDownloadManager.FileDenied( filename, requestID );
}
// char *DL_SharedVarArgs(char *a1, ...)
// {
// 	va_list va; // [sp+8h] [bp+8h]@1
// 	
// 	va_start(va, a1);
// 	dlvar_count = (dlvar_count + 1) % 4;
// 	_vsnprintf(&dlvararg[1024 * dlvar_count], 0x400u, a1, va);
// 	return &dlvararg[1024 * dlvar_count];
// }
//--------------------------------------------------------------------------------------------------------------
//extern ConVar sv_downloadurl;
void CL_QueueDownload( const char *filename )
{
	TheDownloadManager.Queue( CL_GetURL(), filename );
}

//--------------------------------------------------------------------------------------------------------------
int CL_GetDownloadQueueSize(void)
{
//	Con_DPrintf("\n Queue Size %d \n",TheDownloadManager.GetQueueSize());
	return TheDownloadManager.GetQueueSize();
}

//--------------------------------------------------------------------------------------------------------------
int CL_CanUseHTTPDownload(void)
{
	if ( CL_GetURL() != 0 )
	{
	//Con_DPrintf("\n Can Use http: %s \n",CL_GetURL());
	TheDownloadManager.HasMapBeenDownloadedFromServer( va( "%s:%s", CL_GetURL(), CL_MapName) );
	return 1;
	}
	return 0;
}

//--------------------------------------------------------------------------------------------------------------
void CL_MarkMapAsUsingHTTPDownload(void)
{
	TheDownloadManager.MarkMapAsDownloadedFromServer( va( "%s:%s", CL_GetURL(), CL_MapName ) );
}
