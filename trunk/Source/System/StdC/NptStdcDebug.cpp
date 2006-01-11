/*****************************************************************
|
|      File: NptStdcDebug.c
|
|      Atomix - Debug Support: StdC Implementation
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

#if defined(NPT_CONFIG_HAVE_VSNPRINTF)
    vsnprintf(buffer, sizeof(buffer), format, args);
#elif defined(NPT_CONFIG_HAVE_VSPRINTF)
    vsprintf(buffer, format, args);
#else
    buffer[0] = '\0';
#endif
    NPT_Print(buffer);

    va_end(args);
#else
    NPT_COMPILER_UNUSED(format);
#endif
}
