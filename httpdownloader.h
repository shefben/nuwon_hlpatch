#ifndef httpdownloader_h__
#define httpdownloader_h__

#include <assert.h>

#include "download.h"
#include "download_internal.h"
#include "KeyValues.h"
#include "bzip2/bzlib.h"
#include "Color.h"
#include "UtlVector.h"
#include "hooked_client.h"

//--------------------------------------------------------------------------------------------------------------
// Purpose: Implements download cache manager
//--------------------------------------------------------------------------------------------------------------
class DownloadCache
{
public:
	DownloadCache();
	~DownloadCache();
	void Init();
	
	void GetCachedData( RequestContext *rc );			///< Loads cached data, if any
	void PersistToDisk( const RequestContext *rc );		///< Writes out a completed download to disk
	void PersistToCache( const RequestContext *rc );	///< Writes out a partial download (lost connection, user abort, etc) to cache
	
private:
	KeyValues *m_cache;
	
	void GetCacheFilename( const RequestContext *rc, char cachePath[_MAX_PATH] );
	void GenerateCacheFilename( const RequestContext *rc, char cachePath[_MAX_PATH] );
	
	void BuildKeyNames( const char *gamePath );			///< Convenience function to build the keys to index into m_cache
	char m_cachefileKey[BufferSize + 64];
	char m_timestampKey[BufferSize + 64];
};
static DownloadCache *TheDownloadCache = NULL;


//--------------------------------------------------------------------------------------------------------------
// Purpose: Implements download manager class
//--------------------------------------------------------------------------------------------------------------
class DownloadManager
{
public:
	DownloadManager();
	~DownloadManager();
	
	void Queue( const char *baseURL, const char *gamePath );
	void Stop() { Reset(); }
	int GetQueueSize() { return m_queuedRequests.Count(); }
	int GetTotalPercentage() {return totalfileprogress;}
	int GetFilePercentage() {return dlprogress;}
	void UpdateProgressBar();
	
	bool Update();	///< Monitors download thread, starts new downloads, and updates progress bar
	void SetUrl(const char* url){svurl = url;}
	bool FileReceived( const char *filename, unsigned int requestID );
	bool FileDenied( const char *filename, unsigned int requestID );
	const char* GetUrl(){if(sizeof(svurl) > 0)
								return svurl;
							else 
								return 0;}
	
	bool HasMapBeenDownloadedFromServer( const char *serverMapName );
	void MarkMapAsDownloadedFromServer( const char *serverMapName );
	void IsThreadFinished();
	int m_failedRequests;
private:
	void Reset();						///< Cancels any active download, as well as any queued ones
	
	void PruneCompletedRequests();		///< Check download requests that have been completed to see if their threads have exited
	void CheckActiveDownload();			///< Checks download status, and updates progress bar
	void StartNewDownload();			///< Starts a new download if there are queued requests
	
    const char* svurl;
	typedef CUtlVector< RequestContext * > RequestVector;
	RequestVector m_queuedRequests;		///< these are requests waiting to be spawned
	RequestContext *m_activeRequest;	///< this is the active request being downloaded in another thread
	RequestVector m_completedRequests;	///< these are waiting for the thread to exit
	int m_lastPercent;					///< last percent value the progress bar was updated with (to avoid spamming it)
	int m_totalRequests;				///< Total number of requests (used to set the top progress bar)
	int filepercentage; //For outside thread, file percentage
	int m_RequestIDCounter;				///< global increasing request ID counter
//	 float progress = 0; // Overall progress
	typedef CUtlVector< char * > StrVector;
	StrVector m_downloadedMaps;			///< List of maps for which we have already tried to download assets.
	
};

//--------------------------------------------------------------------------------------------------------------
static DownloadManager TheDownloadManager;
#endif // httpdownloader_h__
