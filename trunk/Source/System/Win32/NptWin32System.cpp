/*****************************************************************
|
|   Neptune - System :: Win32 Implementation
|
|   (c) 2001-2006 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#if defined(_XBOX)
#include <xtl.h>
#else
#include <windows.h>
#endif

#include <sys/timeb.h>

#include "NptConfig.h"
#include "NptTypes.h"
#include "NptSystem.h"
#include "NptResults.h"
#include "NptDebug.h"

/*----------------------------------------------------------------------
|   NPT_System::GetProcessId
+---------------------------------------------------------------------*/
NPT_Result
NPT_System::GetProcessId(NPT_Integer& id)
{
    //id = getpid();
    id = 0;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_System::GetCurrentTimeStamp
+---------------------------------------------------------------------*/
NPT_Result
NPT_System::GetCurrentTimeStamp(NPT_TimeStamp& now)
{
    struct _timeb time_stamp;

#if (_MSC_VER >= 1400)
    _ftime_s(&time_stamp);
#else
    _ftime(&time_stamp);
#endif
    now.m_Seconds     = (long)time_stamp.time;
    now.m_NanoSeconds = (long)time_stamp.millitm*1000000;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_System::Sleep
+---------------------------------------------------------------------*/
NPT_Result
NPT_System::Sleep(const NPT_TimeInterval& duration)
{
    DWORD milliseconds = 1000*duration.m_Seconds + duration.m_NanoSeconds/1000000;
    ::Sleep(milliseconds);

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_System::SleepUntil
+---------------------------------------------------------------------*/
NPT_Result
NPT_System::SleepUntil(const NPT_TimeStamp& when)
{
    NPT_TimeStamp now;
    GetCurrentTimeStamp(now);
    if (when > now) {
        NPT_TimeInterval duration = when-now;
        return Sleep(duration);
    } else {
        return NPT_SUCCESS;
    }
}

/*----------------------------------------------------------------------
|   NPT_System::SetRandomSeed
+---------------------------------------------------------------------*/
NPT_Result  
NPT_System::SetRandomSeed(unsigned int seed)
{
    srand(seed);
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_System::NPT_System
+---------------------------------------------------------------------*/
NPT_Integer 
NPT_System::GetRandomInteger()
{
    static bool seeded = false;
    if (seeded == false) {
        NPT_TimeStamp now;
        GetCurrentTimeStamp(now);
        srand(now.m_NanoSeconds);
        seeded = true;
    }

    return rand();
}

