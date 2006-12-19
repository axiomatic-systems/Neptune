/*****************************************************************
|
|   Neptune - Console
|
|   (c) 2002-2006 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include <stdarg.h>

#include "NptConfig.h"
#include "NptConsole.h"
#include "NptUtils.h"

/*----------------------------------------------------------------------
|   NPT_ConsoleOutputFunction
+---------------------------------------------------------------------*/
static void
NPT_ConsoleOutputFunction(void* parameter, const char* message)
{
    NPT_ConsoleOutput(message);
}

/*----------------------------------------------------------------------
|   NPT_ConsoleOutputF
+---------------------------------------------------------------------*/
void 
NPT_ConsoleOutputF(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    
    NPT_FormatOutput(NPT_ConsoleOutputFunction, NULL, format, args);

    va_end(args);
}

