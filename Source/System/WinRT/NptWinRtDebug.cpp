/*****************************************************************
|
|   Neptune - Debug Support: WinRt Implementation
|
|   (c) 2002-2012 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include <Windows.h>

#include "NptConfig.h"
#include "NptDefs.h"
#include "NptTypes.h"
#include "NptDebug.h"

/*----------------------------------------------------------------------
|   NPT_DebugOutput
+---------------------------------------------------------------------*/
void
NPT_DebugOutput(const char* message)
{
    OutputDebugString(message);
}

