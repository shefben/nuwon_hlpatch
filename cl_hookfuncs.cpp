/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
** Date: 2015/04/29 23:24
** File:cl_hookfuncs.cpp
** Description: Contains The functions that need to be hooked to take control
** of.
**
** Author: Benjamin Shefte
** TODO:
**
** -------------------------------------------------------------------------*/


#include "utils.h"
#include "download.h"
#include "hooked_client.h"


int __cdecl IsSafeFileToDownload(char *a1)
{
  int v1; // esi@1
  char *v3; // edi@8
  signed int v4; // ecx@10
  bool v5; // zf@12
  char haystack; // [sp+10h] [bp-100Ch]@2

  v1 = 0;
  if ( a1 )
  {
    Q_strncpy(&haystack, a1, 0x1000u);
    strlwr(&haystack);
	if (!Q_strstr(&haystack, ".wad"))
	{
		MessageBox(NULL, "wad2", "test", MB_OK);

	}

    if ( !Q_strstr(&haystack, "\\")
      && !Q_strstr(&haystack, ":")
      && !Q_strstr(&haystack, "..")
      && !Q_strstr(&haystack, "~")
      && haystack != 47 )
    {
      v3 = strchr(&haystack, 46);
      if ( v3 == strrchr(&haystack, 46) )
      {
        if ( v3 )
        {
          v4 = -1;
          do
          {
            if ( !v4 )
              break;
            v5 = *v3++ == 0;
            --v4;
          }
          while ( !v5 );
          if ( v4 == -6
            && !Q_stristr(&haystack, ".cfg")
            && !Q_stristr(&haystack, ".lst")
            && !Q_stristr(&haystack, ".exe")
            && !Q_stristr(&haystack, ".vbs")
            && !Q_stristr(&haystack, ".com")
            && !Q_stristr(&haystack, ".bat")
            && !Q_stristr(&haystack, ".dll")
            && !Q_stristr(&haystack, ".ini")
            && !Q_stristr(&haystack, ".log")
            && !Q_stristr(&haystack, "halflife.wad")
            && !Q_stristr(&haystack, "pak0.pak")
            && !Q_stristr(&haystack, "xeno.wad")
           // && !Q_stristr(&haystack, off_26D22B)
            && !Q_stristr(&haystack, ".dylib") )
            v1 = Q_stristr(&haystack, ".sys") == 0;
        }
      }
    }
  }
  return v1;
}

