#ifndef utils_h__
#define utils_h__
#pragma once
#include <string.h>
#include "Defines.h"


// Prevent tons of unused windows definitions
#ifdef _WIN32
#include "windows.h"
#else //_WIN32
#define FALSE 0
#define TRUE (!FALSE)
typedef UINT32 ULONG;
typedef unsigned char BYTE;
typedef int BOOL;
#define MAX_PATH PATH_MAX
#include <limits.h>
#include <stdarg.h>
#include <string.h> // memset
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#endif //_WIN32
// #define _vsnprintf(a,b,c,d) vsnprintf(a,b,c,d)
#ifdef _WIN32
#define CORRECT_PATH_SEPARATOR '\\'
#define INCORRECT_PATH_SEPARATOR '/'
#elif _LINUX
#define CORRECT_PATH_SEPARATOR '/'
#define INCORRECT_PATH_SEPARATOR '\\'
#endif
#define PATHSEPARATOR(c) ((c) == '/')
#define COPY_ALL_CHARACTERS -1
// does a varargs printf into a temp buffer
char	*va(char *format, ...);
void print_ip(int ip);
void placeJMP(BYTE *, DWORD, DWORD);
typedef struct characterset_s
{
	char set[256];
} characterset_t;

// wordbreak parsing set
static characterset_t	g_BreakSet, g_BreakSetIncludingColons;

#define COM_TOKEN_MAX_LENGTH 1024
#define IN_CHARACTERSET( SetBuffer, character ) ((SetBuffer).set[(character)])

char * strtok_s(char *str, const char *delim, char **ctx);
// These are versions of functions that guarantee null termination.
//
// maxLen is the maximum number of bytes in the destination string.
// pDest[maxLen-1] is always null terminated if pSrc's length is >= maxLen.
//
// This means the last parameter can usually be a sizeof() of a string.
void Q_strncpy( char *pDest, const char *pSrc, int maxLen );
// char *Q_strncpy(char *dest, const char *src, size_t n);
char *COM_Parse (char *data);
int  Q_snprintf( char *pDest, int destLen, char const *pFormat, ... );
char *Q_strncat(char *, const char *, size_t);
char *Q_strnlwr(char *, size_t);
char *Q_stristr( char* pStr, char const* pSearch );
char const*	Q_stristr( char const* pStr, char const* pSearch );
void Q_AppendSlash( char *pStr, int strSize );
void Q_FixSlashes( char *pname, char separator );
int  Q_vsnprintf( char *buffer, size_t buffersize, const char *format, va_list args );
void Q_MakeAbsolutePath( char *pOut, int outLen, const char *pPath, const char *pStartingDir );
// Make path end with extension if it doesn't already have an extension
void Q_StripExtension( const char *, char *, int );
bool Q_RemoveDotSlashes( char *pFilename, char separator );


inline char*	Q_strstr( const char *s1, const char *search )		{ return strstr( s1, search ); }
//inline void		Q_memset (void *dest, int fill, int count)			{ memset( dest, fill, count ); }
inline void		Q_memcpy (void *dest, const void *src, int count)	{ memcpy( dest, src, count ); }
inline void		Q_memmove (void *dest, const void *src, int count)	{ memmove( dest, src, count ); }
inline int		Q_memcmp (void *m1, void *m2, int count)			{ return memcmp( m1, m2, count ); } 
inline int		Q_strlen (const char *str)							{ return strlen ( str ); }
inline void		Q_strcpy (char *dest, const char *src)				{ strcpy( dest, src ); }
inline char*	Q_strrchr (const char *s, char c)					{ return strrchr( s, c ); }
inline int		Q_strcmp (const char *s1, const char *s2)			{ return strcmp( s1, s2 ); }
inline int		Q_stricmp( const char *s1, const char *s2 )			{ return stricmp( s1, s2 ); }
inline char*	Q_strupr (char *start)								{ return strupr( start ); }
inline char*	Q_strlower (char *start)							{ return strlwr( start ); }

//-----------------------------------------------------------------------------
// Methods to invoke the constructor, copy constructor, and destructor
//-----------------------------------------------------------------------------

template <class T> 
inline void Construct( T* pMemory )
{
	new( pMemory ) T;
}

template <class T> 
inline void CopyConstruct( T* pMemory, T const& src )
{
	new( pMemory ) T(src);
}

template <class T> 
inline void Destruct( T* pMemory )
{
	pMemory->~T();
	
#ifdef _DEBUG
	memset( pMemory, 0xDD, sizeof(T) );
#endif
}
#endif // utils_h__