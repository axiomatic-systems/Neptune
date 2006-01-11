/*****************************************************************
|
|      Neptune - System :: Win32 Implementation
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <windows.h>
#include <sys/timeb.h>

#include "NptConfig.h"
#include "NptTypes.h"
#include "NptSystem.h"
#include "NptResults.h"
#include "NptDebug.h"

/*----------------------------------------------------------------------
|       globals
+---------------------------------------------------------------------*/
NPT_System System;

/*----------------------------------------------------------------------
|       NPT_Win32System
+---------------------------------------------------------------------*/
class NPT_Win32System : public NPT_SystemInterface
{
public:
    // methods
                NPT_Win32System();
               ~NPT_Win32System();
    NPT_Result  GetProcessId(NPT_Integer& id);
    NPT_Result  GetCurrentTimeStamp(NPT_TimeStamp& now);
    NPT_Result  Sleep(const NPT_TimeInterval& duration);
    NPT_Result  SleepUntil(const NPT_TimeStamp& when);
    NPT_Result  SetRandomSeed(unsigned int seed);
    NPT_Integer GetRandomInteger();
};

/*----------------------------------------------------------------------
|       NPT_Win32System::NPT_Win32System
+---------------------------------------------------------------------*/
NPT_Win32System::NPT_Win32System()
{
}

/*----------------------------------------------------------------------
|       NPT_Win32System::~NPT_Win32System
+---------------------------------------------------------------------*/
NPT_Win32System::~NPT_Win32System()
{
}

/*----------------------------------------------------------------------
|       NPT_Win32System::GetProcessId
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32System::GetProcessId(NPT_Integer& id)
{
    //id = getpid();
    id = 0;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Win32System::GetCurrentTimeStamp
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32System::GetCurrentTimeStamp(NPT_TimeStamp& now)
{
    struct _timeb time_stamp;

    _ftime(&time_stamp);
    now.m_Seconds     = time_stamp.time;
    now.m_NanoSeconds = time_stamp.millitm*1000000;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Win32System::Sleep
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32System::Sleep(const NPT_TimeInterval& duration)
{
    DWORD milliseconds = 1000*duration.m_Seconds + duration.m_NanoSeconds/1000000;
    ::Sleep(milliseconds);

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Win32System::SleepUntil
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32System::SleepUntil(const NPT_TimeStamp& when)
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
|       NPT_Win32System::SetRandomSeed
+---------------------------------------------------------------------*/
NPT_Result  
NPT_Win32System::SetRandomSeed(unsigned int seed)
{
    srand(seed);
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_System::NPT_System
+---------------------------------------------------------------------*/
NPT_Integer 
NPT_Win32System::GetRandomInteger()
{
    return rand();
}

/*----------------------------------------------------------------------
|       NPT_System::NPT_System
+---------------------------------------------------------------------*/
NPT_System::NPT_System()
{
    m_Delegate = new NPT_Win32System();
}

