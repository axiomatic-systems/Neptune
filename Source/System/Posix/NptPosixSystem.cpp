/*****************************************************************
|
|      Neptune - System :: Posix Implementation
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

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
|       NPT_PosixSystem
+---------------------------------------------------------------------*/
class NPT_PosixSystem : public NPT_SystemInterface
{
public:
    // methods
                NPT_PosixSystem();
               ~NPT_PosixSystem();
    NPT_Result  GetProcessId(NPT_Integer& id);
    NPT_Result  GetCurrentTimeStamp(NPT_TimeStamp& now);
    NPT_Result  Sleep(const NPT_TimeInterval& duration);
    NPT_Result  SleepUntil(const NPT_TimeStamp& when);
    NPT_Result  SetRandomSeed(unsigned int seed);
    NPT_Integer GetRandomInteger();

private:
    // members
    pthread_mutex_t m_SleepMutex;
    pthread_cond_t  m_SleepCondition;
};

/*----------------------------------------------------------------------
|       NPT_PosixSystem::NPT_PosixSystem
+---------------------------------------------------------------------*/
NPT_PosixSystem::NPT_PosixSystem()
{
    pthread_mutex_init(&m_SleepMutex, NULL);
    pthread_cond_init(&m_SleepCondition, NULL);
}

/*----------------------------------------------------------------------
|       NPT_PosixSystem::~NPT_PosixSystem
+---------------------------------------------------------------------*/
NPT_PosixSystem::~NPT_PosixSystem()
{
    pthread_cond_destroy(&m_SleepCondition);
    pthread_mutex_destroy(&m_SleepMutex);
}

/*----------------------------------------------------------------------
|       NPT_PosixSystem::GetProcessId
+---------------------------------------------------------------------*/
NPT_Result
NPT_PosixSystem::GetProcessId(NPT_Integer& id)
{
    id = getpid();
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_PosixSystem::GetCurrentTimeStamp
+---------------------------------------------------------------------*/
NPT_Result
NPT_PosixSystem::GetCurrentTimeStamp(NPT_TimeStamp& now)
{
    struct timeval now_tv;

    // get current time from system
    if (gettimeofday(&now_tv, NULL)) {
        now.m_Seconds     = 0;
        now.m_NanoSeconds = 0;
        return NPT_FAILURE;
    }
    
    // convert format
    now.m_Seconds     = now_tv.tv_sec;
    now.m_NanoSeconds = now_tv.tv_usec * 1000;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_PosixSystem::Sleep
+---------------------------------------------------------------------*/
NPT_Result
NPT_PosixSystem::Sleep(const NPT_TimeInterval& duration)
{
    struct timespec time_req;
    struct timespec time_rem;
    int             result;

    // setup the time value
    time_req.tv_sec  = duration.m_Seconds;
    time_req.tv_nsec = duration.m_NanoSeconds;

    // sleep
    do {
        result = nanosleep(&time_req, &time_rem);
        time_req = time_rem;
    } while (result == -1 && errno == EINTR);

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_PosixSystem::SleepUntil
+---------------------------------------------------------------------*/
NPT_Result
NPT_PosixSystem::SleepUntil(const NPT_TimeStamp& when)
{
    struct timespec timeout;
    int             result;

    // setup timeout
    timeout.tv_sec  = when.m_Seconds;
    timeout.tv_nsec = when.m_NanoSeconds;

    // sleep
    do {
        result = pthread_cond_timedwait(&m_SleepCondition, 
                                        &m_SleepMutex, 
                                        &timeout);
        if (result == ETIMEDOUT) {
            return NPT_SUCCESS;
        }
    } while (result == EINTR);

    return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|       NPT_PosixSystem::SetRandomSeed
+---------------------------------------------------------------------*/
NPT_Result  
NPT_PosixSystem::SetRandomSeed(unsigned int seed)
{
    srand(seed);
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_System::NPT_System
+---------------------------------------------------------------------*/
NPT_Integer 
NPT_PosixSystem::GetRandomInteger()
{
    return rand();
}

/*----------------------------------------------------------------------
|       NPT_System::NPT_System
+---------------------------------------------------------------------*/
NPT_System::NPT_System()
{
    m_Delegate = new NPT_PosixSystem();
}

