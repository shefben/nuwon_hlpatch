#include "Crc32.h"

#include "Defines.h"
#include <winbase.h>
//CheckSum Files
const char* openglF = "opengl32.dll";

//Checksum Code
unsigned long openglcrc = 0x36D26100;


void checkCrc32()
{
    CCRC32 MyCRC32;
    MyCRC32.Initialize();
    unsigned long ulCRCP = MyCRC32.FileCRC(openglF);
	
	
	if(ulCRCP != openglcrc)
    {

        MessageBoxA(0, (const char*)ulCRCP, "Error", MB_OK | MB_ICONSTOP);
        ExitProcess(1);
    }

}
#if 0

void checkDebugger1()
{
	if (IsDebuggerPresent())
	{
        std::cout << "Stop debugging program!" << std::endl;
        exit(-1);
    } 
}
LONG CALLBACK ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo)
	{
    PCONTEXT ctx = ExceptionInfo->ContextRecord;
    if (ctx->Dr0 != 0 || ctx->Dr1 != 0 || ctx->Dr2 != 0 || ctx->Dr3 != 0)
		{
        std::cout << "Stop debugging program!" << std::endl;
        exit(-1);
		}
    ctx->Eip += 2;
    return EXCEPTION_CONTINUE_EXECUTION;
} 
BOOL g_isDebuggerPresent = TRUE;
EXCEPTION_DISPOSITION ExceptionRoutine(
    PEXCEPTION_RECORD ExceptionRecord,
    PVOID             EstablisherFrame,
    PCONTEXT          ContextRecord,
    PVOID             DispatcherContext)
{
    g_isDebuggerPresent = FALSE;
    ContextRecord->Eip += 1;
    return ExceptionContinueExecution;
}
int checkDebugger2()
{
    __asm
    {
        // set SEH handler
        push ExceptionRoutine
        push dword ptr fs:[0]
        mov  dword ptr fs:[0], esp
        // generate interrupt
        int  3h
        // return original SEH handler
        mov  eax, [esp]
        mov  dword ptr fs:[0], eax
        add  esp, 8
    }
	
    AddVectoredExceptionHandler(0, ExceptionHandler);
    __asm int 1h; 
    if (g_isDebuggerPresent)
    {
        std::cout << "Stop debugging program!" << std::endl;
        exit(-1);
    }
    return 0
}

#include <windows.h>
#include <gl/gl.h>

// color stuff
#include <vector>
#include "anti_cheat.h"

typedef void (APIENTRY *Tgl1ui) (unsigned int);
typedef void (APIENTRY *Tgl3f)( float , float , float );
typedef void (APIENTRY *Tgl4f)( float , float , float , float );
typedef void (APIENTRY *Tgl2ui) (unsigned int, unsigned int);
typedef void (APIENTRY *TglTexEnvi) (unsigned int, unsigned int, int);
typedef void (APIENTRY *Tglvui) (unsigned int,float*);
typedef void (APIENTRY *xglViewport) (GLint x, GLint y, GLsizei width, GLsizei height);
typedef BOOL (APIENTRY *xwglSwapBuffers) (HDC);
typedef void (APIENTRY *xglBlendFunc) (GLenum sfactor, GLenum dfactor);
typedef void (APIENTRY *xglColor4f) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (APIENTRY *xglLineWidth) (GLfloat width);
typedef void (APIENTRY *TglRectiFunc) (GLint x, GLint y, GLint x2, GLint y2);

Tgl1ui p_fnglBegin = NULL;
Tgl1ui p_fnglDisable= NULL;
Tgl1ui p_fnglEnable= NULL;
Tgl3f p_fnglColor3f = NULL;
Tgl3f p_fnglVertex3f = NULL;
TglTexEnvi p_fnglTexEnvi=NULL;
Tgl2ui p_fnglPolygonMode= NULL;
Tgl4f p_fnglClearColor=NULL;
Tgl1ui p_fnglClear=NULL;
Tglvui p_fnglGetFloatv=NULL;
xglViewport oglViewport=NULL;
xwglSwapBuffers owglSwapBuffers=NULL;
xglColor4f oglColor4f=NULL;
xglBlendFunc oglBlendFunc=NULL;
xglLineWidth oglLineWidth=NULL;
TglRectiFunc oglRecti = NULL;


//========================================================================
void APIENTRY pglBlendFunc(GLenum sfactor, GLenum dfactor)
{
		oglBlendFunc(sfactor, dfactor);
}

void APIENTRY pglBegin(unsigned int mode)
{
(*p_fnglBegin)(mode);
}

void APIENTRY pglVertex3f( float x, float y, float z) 
{
 (*p_fnglVertex3f)(x, y, z);
}


void APIENTRY pglClear (GLbitfield mask)
{

		p_fnglClear(mask);
}

void CheckForOpenGlHook(FARPROC* pProc,LPCTSTR lpProcName)
{
	if (!strcmp(lpProcName,"glDisable")) 
	{
		p_fnglDisable = (Tgl1ui)*pProc;
	} else 
	if (!strcmp(lpProcName,"glEnable")) 
	{
		p_fnglEnable = (Tgl1ui)*pProc;
	} else 
    if (!strcmp(lpProcName,"glColor4f")) 
	{
		oglColor4f = (xglColor4f)*pProc;
	} else 
	if(!strcmp(lpProcName,"glViewport"))
	{
		oglViewport = (xglViewport)*pProc;
	} else
	if (!strcmp(lpProcName,"glBlendFunc")) 
	{
		oglBlendFunc = (xglBlendFunc)*pProc;
		*pProc = (FARPROC)&pglBlendFunc;
	} else 
	if (!strcmp(lpProcName,"glLineWidth")) 
	{
		oglLineWidth = (xglLineWidth)*pProc;
	} else 
	if (!strcmp(lpProcName,"glClear")) 
	{
		p_fnglClear = (Tgl1ui)*pProc;
		*pProc = (FARPROC)&pglClear;
	} else 
	if (!strcmp(lpProcName,"glTexEnvi")) 
	{
		p_fnglTexEnvi = (TglTexEnvi)*pProc;
	} else 
	if (!strcmp(lpProcName,"glColor3f")) 
	{
		p_fnglColor3f = (Tgl3f)*pProc;
	} else 
	if (!strcmp(lpProcName,"glPolygonMode")) 
	{
		p_fnglPolygonMode = (Tgl2ui)*pProc;
	} else 
	if (!strcmp(lpProcName,"glRecti")) 
	{
		oglRecti = (TglRectiFunc)*pProc;
	} else 
    if (!strcmp(lpProcName,"glGetFloatv")) 
	{
		p_fnglGetFloatv = (Tglvui)*pProc;
    } else 
    if (!strcmp(lpProcName,"glVertex3f")) 
	{
		p_fnglVertex3f = (Tgl3f)*pProc;
		*pProc = (FARPROC)&pglVertex3f;
	} else 
	if (!strcmp(lpProcName,"glBegin")) 
	{
		p_fnglBegin = (Tgl1ui)*pProc;
		*pProc = (FARPROC)&pglBegin;
	} else 
	if (!strcmp(lpProcName,"glClearColor")) 
	{ 
		p_fnglClearColor = (Tgl4f)*pProc;
	}	
}

bool FileExists( const std::string &Filename )
{
    return access( Filename.c_str(), 0 ) == 0;
}
/*
BOOL (WINAPI *Module32NextPtr) ( HANDLE hSnapshot, LPMODULEENTRY32 lpme );
BOOL WINAPI newModule32Next ( HANDLE hSnapshot, LPMODULEENTRY32 lpme )
{
  BOOL res = (*Module32NextPtr)(hSnapshot,lpme);
  if (lpme->hModule == xModule ) res = (*Module32NextPtr)(hSnapshot,lpme);
  return res;
}

HMODULE WINAPI newLoadLibrary ( LPCSTR lpLibFileName );
DWORD lloffset;
DWORD llpt=(DWORD)&newLoadLibrary;

typedef HINSTANCE (WINAPI *LoadLibraryFunc)(LPCTSTR);
LoadLibraryFunc LoadLibraryPtr;

FARPROC WINAPI newGetProcAddress ( HMODULE hModule, LPCSTR lpProcName );
DWORD gpoffset;
DWORD gppt=(DWORD)&newGetProcAddress;

typedef FARPROC (WINAPI *GetProcAddressFunc)(HMODULE,LPCTSTR);

FARPROC  WINAPI newGetProcAddress ( HMODULE hModule, LPCSTR lpProcName )
{
	FARPROC nResult;
	nResult = GetProcAddress(hModule, lpProcName);
	if(HIWORD(lpProcName))
	{
		if(!strcmp(lpProcName,"CreateFileA"))
		{
		
		}
		else if(!strcmp(lpProcName,"QueryPerformanceCounter"))
		{
			QueryPerformanceCounterPtr = (QueryPerformanceCounterFunc)GetProcAddress(hModule,lpProcName);
			return (FARPROC)pQueryPerformanceCounter;

		}
		else if(!strcmp(lpProcName,"GetProcAddress"))
		{
			return (FARPROC)newGetProcAddress;
            
		}
		else if(!strcmp(lpProcName,"GetMessageA"))
		{
	
		}
		else
		{
			void CheckForOpenGlHook(FARPROC* pProc,LPCTSTR lpProcName);
			CheckForOpenGlHook(&nResult, lpProcName);

			void CheckClientHook(FARPROC* pProc,LPCTSTR lpProcName);
			CheckClientHook(&nResult, lpProcName);
		}
	}
	return nResult;
}

//=====================================================================================================
void ReRoute(HMODULE res,PIMAGE_NT_HEADERS pNTHeader,DWORD* new_addr,char* modulename, char* funcname, DWORD* out_offset)
{
	//generic call to old_add patcher
	bool success = importoffset((char*)res,modulename,funcname,out_offset)!=0;
	if(!success)
	{
		MessageBox(0,funcname,"REROUTE FAILED",0);
		return;
	}
	BYTE *pos;
	BYTE* codestart=(BYTE*)res+pNTHeader->OptionalHeader.BaseOfCode;
	DWORD length=pNTHeader->OptionalHeader.SizeOfCode;
	while (ScanPattern_FF15calladr(codestart,*out_offset,length,&pos))
	{
		length-=pos+6-codestart;
		codestart=pos+6;
		Protect(pos+2,sizeof(DWORD));
		DWORD* blub0r=(DWORD*)(pos+2);
		*blub0r = (DWORD)new_addr; 
		Protect(pos+2,-1);
	}
	codestart=(BYTE*)res+pNTHeader->OptionalHeader.BaseOfCode;
	length=pNTHeader->OptionalHeader.SizeOfCode;
	while (ScanPattern_8B2Dcalladr(codestart,*out_offset,length,&pos))
	{
		length-=pos+6-codestart;
		codestart=pos+6;
		Protect(pos+2,sizeof(DWORD));
		DWORD* blub0r=(DWORD*)(pos+2);
		*blub0r = (DWORD)new_addr; 
		Protect(pos+2,-1);
	}
	codestart=(BYTE*)res+pNTHeader->OptionalHeader.BaseOfCode;
	length=pNTHeader->OptionalHeader.SizeOfCode;
	while (ScanPattern_8B35calladr(codestart,*out_offset,length,&pos))
	{
		length-=pos+6-codestart;
		codestart=pos+6;
		Protect(pos+2,sizeof(DWORD));
		DWORD* blub0r=(DWORD*)(pos+2);
		*blub0r = (DWORD)new_addr; 
		Protect(pos+2,-1);
	}
}

HMODULE WINAPI newLoadLibrary  ( LPCSTR lpLibFileName )
{
	LoadLibraryPtr = (LoadLibraryFunc)(*(DWORD*)lloffset); 
	HMODULE res = (*LoadLibraryPtr)(lpLibFileName); 
	if (strstr("client.dll",lpLibFileName))
	{
		InterceptDllCall(res,"Kernel32.dll","Module32Next",&newModule32Next,(VOID**)&Module32NextPtr,NULL);
	}
	return res;
}*/
#endif