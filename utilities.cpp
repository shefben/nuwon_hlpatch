/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
** Date: 2015/04/29 23:25
** File:utilities.cpp
** Description: Special functions which  are Misc and/or used through-out the 
** project and don't belong anywhere else
**
** Author: Benjamin Shefte
** TODO:
**
** -------------------------------------------------------------------------*/

#include "utils.h"
#include <direct.h>
#include "hooked_funcs.h"

char *  strtok_s(char *str, const char *delim, char **ctx)
{
	if(!delim || !ctx || (!str && !*ctx)) {
		   // MSVCRT__invalid_parameter(NULL, NULL, NULL, 0, 0);
		   // *MSVCRT__errno() = MSVCRT_EINVAL;
		    return NULL;
		}
	
		if(!str)
		    str = *ctx;
	
		while(*str && strchr(delim, *str))
		    str++;
	if(!*str)
		    return NULL;
	
		*ctx = str+1;
	while(**ctx && !strchr(delim, **ctx))
		    (*ctx)++;
	if(**ctx)
		    *(*ctx)++ = 0;
	
		return str;
}
/*
==============
COM_Parse

  Parse a token out of a string
  ==============
*/
char	com_token[COM_TOKEN_MAX_LENGTH];

char *COM_Parse (char *data)
{
	int             c;
	int             len;
	characterset_t	*breaks;
	
	breaks = &g_BreakSetIncludingColons;
	if ( com_ignorecolons )
		breaks = &g_BreakSet;
	
	len = 0;
	com_token[0] = 0;
	
	if (!data)
		return NULL;
	
	// skip whitespace
skipwhite:
	while ( (c = *data) <= ' ')
	{
		if (c == 0)
			return NULL;                    // end of file;
		data++;
	}
	
	// skip // comments
	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}
	
	
	// handle quoted strings specially
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c=='\"' || !c)
			{
				com_token[len] = 0;
				return data;
			}
			com_token[len] = c;
			len++;
		}
	}
	
	// parse single characters
	if ( IN_CHARACTERSET( *breaks, c ) )
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data+1;
	}
	
	// parse a regular word
	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
		if ( IN_CHARACTERSET( *breaks, c ) )
			break;
	} while (c>32);
	
	com_token[len] = 0;
	return data;
}



/*
============
va

  does a varargs printf into a temp buffer, so I don't need to have
  varargs versions of all text functions.
  FIXME: make this buffer size safe someday
  ============
*/
char    *va(char *format, ...)
{
	va_list         argptr;
	static char             string[4][1024];
	static int curstring = 0;
	
	curstring = ( curstring + 1 ) % 4;
	
	va_start (argptr, format);
#ifdef _WIN32
	Q_vsnprintf( string[curstring], sizeof( string[curstring] ), format, argptr );
#else
	vsprintf (string[curstring], format,argptr);
#endif
	va_end (argptr);
	
	return string[curstring];  
}
void print_ip(int ip) {
	unsigned char bytes[4];
	int i;
	for(i=0; i<4; i++) {
		bytes[i] = (ip >> i*8) & 0xFF;
	}
	Con_Printf("%d.%d.%d.%d\n", bytes[3], bytes[2], bytes[1], bytes[0]);
}

void placeJMP(BYTE *pAddress, DWORD dwJumpTo, DWORD dwLen){
	DWORD dwOldProtect, dwBkup, dwRelAddr;
	
	// Basic VirtualProtect... y'all should know this
	VirtualProtect(pAddress, dwLen, PAGE_EXECUTE_READWRITE, &dwOldProtect);
    
	// Calculate the "distance" we're gonna have to jump - the size of the JMP instruction
	dwRelAddr = (DWORD) (dwJumpTo - (DWORD) pAddress) - 5;    
	
	// Write the JMP opcode @ our jump position...
	*pAddress = 0xE9;
	
	// Write the offset to where we're gonna jump
	*((DWORD *)(pAddress + 0x1)) = dwRelAddr; 
	
	// Overwrite the rest of the bytes with NOPs
	for(DWORD x = 0x5; x < dwLen; x++)
		*(pAddress + x) = 0x90; 
	
	// Restore the default permissions
	VirtualProtect(pAddress, dwLen, dwOldProtect, &dwBkup);
	
}


char *Q_strnlwr(char *s, size_t count)
{
	char* pRet = s;
	if (!s)
		return s;
	
	while (--count >= 0)
	{
		if (!*s)
			break;
		
		*s = tolower(*s);
		++s;
	}
	
	return pRet;
}
void Q_strncpy( char *pDest, char const *pSrc, int maxLen )
{	
	strncpy( pDest, pSrc, maxLen );
	if( maxLen )
		pDest[maxLen-1] = 0;
}
int Q_vsnprintf( char *pDest, unsigned int maxLen, char const *pFormat, char* params )
{	
	int len = _vsnprintf( pDest, maxLen, pFormat, params );
	
	if( len < 0 )
	{
		len = maxLen;
		pDest[maxLen-1] = 0;
	}
	
	return len;
}

int Q_snprintf( char *pDest, int maxLen, char const *pFormat, ... )
{	
	va_list marker;
	
	va_start( marker, pFormat );
	int len = _vsnprintf( pDest, maxLen, pFormat, marker );
	va_end( marker );
	
	// Len < 0 represents an overflow
	if( len < 0 )
	{
		len = maxLen;
		pDest[maxLen-1] = 0;
	}
	
	return len;
}
#ifdef LINUX
inline char *strlwr( char *start )
{
	char *str = start;
	while( str && *str )
	{
		*str = (char)tolower(*str);
		str++;
	}
	return start;
}
#endif
//-----------------------------------------------------------------------------
// Finds a string in another string with a case insensitive test
//-----------------------------------------------------------------------------

char const* Q_stristr( char const* pStr, char const* pSearch )
{
	
	if (!pStr || !pSearch) 
		return 0;
	
	char const* pLetter = pStr;
	
	// Check the entire string
	while (*pLetter != 0)
	{
		// Skip over non-matches
		if (tolower(*pLetter) == tolower(*pSearch))
		{
			// Check for match
			char const* pMatch = pLetter + 1;
			char const* pTest = pSearch + 1;
			while (*pTest != 0)
			{
				// We've run off the end; don't bother.
				if (*pMatch == 0)
					return 0;
				
				if (tolower(*pMatch) != tolower(*pTest))
					break;
				
				++pMatch;
				++pTest;
			}
			
			// Found a match!
			if (*pTest == 0)
				return pLetter;
		}
		
		++pLetter;
	}
	
	return 0;
}

char* Q_stristr( char* pStr, char const* pSearch )
{	
	return (char*)Q_stristr( (char const*)pStr, pSearch );
}


//-----------------------------------------------------------------------------
// Purpose: Changes all '/' or '\' characters into separator
// Input  : *pname - 
//			separator - 
//-----------------------------------------------------------------------------
void Q_FixSlashes( char *pname, char separator  )
{
	while ( *pname )
	{
		if ( *pname == INCORRECT_PATH_SEPARATOR || *pname == CORRECT_PATH_SEPARATOR )
		{
			*pname = separator;
		}
		pname++;
	}
}
void Q_StripExtension( const char *in, char *out, int outSize )
{
	// Find the last dot. If it's followed by a dot or a slash, then it's part of a 
	// directory specifier like ../../somedir/./blah.
	
	// scan backward for '.'
	int end = Q_strlen( in ) - 1;
	while ( end > 0 && in[end] != '.' && !PATHSEPARATOR( in[end] ) )
	{
		--end;
	}
	
	if (end > 0 && !PATHSEPARATOR( in[end] ) && end < outSize)
	{
		int nChars = min( end, outSize-1 );
		if ( out != in )
		{
			memcpy( out, in, nChars );
		}
		out[nChars] = 0;
	}
	else
	{
		// nothing found
		if ( out != in )
		{
			Q_strncpy( out, in, outSize );
		}
	}
}
//-----------------------------------------------------------------------------
// small helper function shared by lots of modules
//-----------------------------------------------------------------------------
bool Q_IsAbsolutePath( const char *pStr )
{
	bool bIsAbsolute = ( pStr[0] && pStr[1] == ':' ) || pStr[0] == '/' || pStr[0] == '\\';
	return bIsAbsolute;
}

void Q_MakeAbsolutePath( char *pOut, int outLen, const char *pPath, const char *pStartingDir )
{
	if ( Q_IsAbsolutePath( pPath ) )
	{
		// pPath is not relative.. just copy it.
		Q_strncpy( pOut, pPath, outLen );
	}
	else
	{
		// Make sure the starting directory is absolute..
		if ( pStartingDir && Q_IsAbsolutePath( pStartingDir ) )
		{
			Q_strncpy( pOut, pStartingDir, outLen );
		}
		else
		{
			if ( !_getcwd( pOut, outLen ) )
				Con_DPrintf( "Q_MakeAbsolutePath: _getcwd failed." );
			
			if ( pStartingDir )
			{
				Q_AppendSlash( pOut, outLen );
				Q_strncat( pOut, pStartingDir, outLen );
			}
		}
		
		// Concatenate the paths.
		Q_AppendSlash( pOut, outLen );
		Q_strncat( pOut, pPath, outLen );
	}
	
	if(!Q_RemoveDotSlashes( pOut, CORRECT_PATH_SEPARATOR) )
		Con_DPrintf( "Q_MakeAbsolutePath: tried to \"..\" past the root." );
	
	Q_FixSlashes( pOut, CORRECT_PATH_SEPARATOR );
}
void Q_AppendSlash( char *pStr, int strSize )
{
	int len = Q_strlen( pStr );
	if ( len > 0 && !PATHSEPARATOR(pStr[len-1]) )
	{
		if ( len+1 >= strSize )
			Con_DPrintf( "V_AppendSlash: ran out of space on %s.", pStr );
		
		pStr[len] = CORRECT_PATH_SEPARATOR;
		pStr[len+1] = 0;
	}
}
bool Q_RemoveDotSlashes( char *pFilename, char separator )
{
	// Remove '//' or '\\'
	char *pIn = pFilename;
	char *pOut = pFilename;
	bool bPrevPathSep = false;
	while ( *pIn )
	{
		bool bIsPathSep = PATHSEPARATOR( *pIn );
		if ( !bIsPathSep || !bPrevPathSep )
		{
			*pOut++ = *pIn;
		}
		bPrevPathSep = bIsPathSep;
		++pIn;
	}
	*pOut = 0;
	
	// Get rid of "./"'s
	pIn = pFilename;
	pOut = pFilename;
	while ( *pIn )
	{
		// The logic on the second line is preventing it from screwing up "../"
		if ( pIn[0] == '.' && PATHSEPARATOR( pIn[1] ) &&
			(pIn == pFilename || pIn[-1] != '.') )
		{
			pIn += 2;
		}
		else
		{
			*pOut = *pIn;
			++pIn;
			++pOut;
		}
	}
	*pOut = 0;
	
	// Get rid of a trailing "/." (needless).
	int len = strlen( pFilename );
	if ( len > 2 && pFilename[len-1] == '.' && PATHSEPARATOR( pFilename[len-2] ) )
	{
		pFilename[len-2] = 0;
	}
	
	// Each time we encounter a "..", back up until we've read the previous directory name,
	// then get rid of it.
	pIn = pFilename;
	while ( *pIn )
	{
		if ( pIn[0] == '.' && 
			pIn[1] == '.' && 
			(pIn == pFilename || PATHSEPARATOR(pIn[-1])) &&	// Preceding character must be a slash.
			(pIn[2] == 0 || PATHSEPARATOR(pIn[2])) )			// Following character must be a slash or the end of the string.
		{
			char *pEndOfDots = pIn + 2;
			char *pStart = pIn - 2;
			
			// Ok, now scan back for the path separator that starts the preceding directory.
			while ( 1 )
			{
				if ( pStart < pFilename )
					return false;
				
				if ( PATHSEPARATOR( *pStart ) )
					break;
				
				--pStart;
			}
			
			// Now slide the string down to get rid of the previous directory and the ".."
			memmove( pStart, pEndOfDots, strlen( pEndOfDots ) + 1 );
			
			// Start over.
			pIn = pFilename;
		}
		else
		{
			++pIn;
		}
	}
	
	Q_FixSlashes( pFilename, separator );	
	return true;
}
char *Q_strncat(char *pDest, const char *pSrc, size_t maxLen)
{
	int len = strlen(pDest);
	maxLen = (maxLen - 1) - len;
	
	if ( maxLen <= 0 )
		return pDest;
	
	char *pOut = strncat( pDest, pSrc, maxLen );
	pOut[len + maxLen] = 0;
	return pOut;
}