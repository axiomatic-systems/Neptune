/*****************************************************************
|
|      File: NptWin32Debug.c
|
|      Neptune - Debug Support: Win32 Implementation
|
|      (c) 2002-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <stdarg.h>
#include <stdio.h>
#include <windows.h>

#include "NptConfig.h"
#include "NptDefs.h"
#include "NptTypes.h"
#include "NptDebug.h"

/*----------------------------------------------------------------------
|       constants
+---------------------------------------------------------------------*/
#define NPT_DEBUG_MAX_BUFFER 512

#if defined(NPT_DEBUG)
/*----------------------------------------------------------------------
|       NPT_Print
+---------------------------------------------------------------------*/
static void
NPT_Print(const char* message)
{
    printf("%s", message);
    OutputDebugString(message);
}
#endif

/*----------------------------------------------------------------------
|       NPT_Debug
+---------------------------------------------------------------------*/
void
NPT_Debug(const char* format, ...)
{
#if defined(NPT_DEBUG)
    char buffer[NPT_DEBUG_MAX_BUFFER];

    va_list args;

    va_start(args, format);

#if (_MSC_VER >= 1400)
	_vsnprintf_s(buffer, sizeof(buffer), sizeof(buffer)-1, format, args);
#else
    _vsnprintf(buffer, sizeof(buffer)-1, format, args);
#endif
    NPT_Print(buffer);

    va_end(args);
#else
    NPT_COMPILER_UNUSED(format);
#endif
}
