/*****************************************************************
|
|      Neptune - Console Support: StdC Implementation
|
|      (c) 2002-2006 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <stdio.h>

#include "NptConfig.h"
#include "NptConsole.h"

/*----------------------------------------------------------------------
|       NPT_ConsoleOutput
+---------------------------------------------------------------------*/
void
NPT_ConsoleOutput(const char* message)
{
    printf("%s", message);
}
