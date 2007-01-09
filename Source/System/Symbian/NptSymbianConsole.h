/*****************************************************************
|
|   Neptune - Console Support: Symbian Implementation
|
|   (c) 2002-2006 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>            // Console

#include "NptConfig.h"
#include "NptResults.h"

/*----------------------------------------------------------------------
|   NPT_Console::Output
+---------------------------------------------------------------------*/
NPT_Result
NPT_Symbian_SetConsole(CConsoleBase* console)
{
    NPT_SymbianConsole = console;
    return NPT_SUCCESS;
}

