/*****************************************************************
|
|      Neptune - System
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

#ifndef _NPT_SYSTEM_H_
#define _NPT_SYSTEM_H_

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "NptTypes.h"
#include "NptTime.h"

/*----------------------------------------------------------------------
|       NPT_SystemInterface
+---------------------------------------------------------------------*/
class NPT_SystemInterface
{
 public:
    // methods
    virtual            ~NPT_SystemInterface() {};
    virtual NPT_Result  GetProcessId(NPT_Integer& id)           = 0;
    virtual NPT_Result  GetCurrentTimeStamp(NPT_TimeStamp& now) = 0;
    virtual NPT_Result  Sleep(const NPT_TimeInterval& duration) = 0;
    virtual NPT_Result  SleepUntil(const NPT_TimeStamp& when)   = 0;
    virtual NPT_Result  SetRandomSeed(unsigned int seed)        = 0;
    virtual NPT_Integer GetRandomInteger()                      = 0;
};

/*----------------------------------------------------------------------
|       NPT_System
+---------------------------------------------------------------------*/
class NPT_System : public NPT_SystemInterface
{
 public:
    // class globals
    static NPT_System& GetSystem() {
        return System;
    }

    // methods
              NPT_System();
             ~NPT_System() { delete m_Delegate; }

    // NPT_SystemInterface methods
    NPT_Result GetProcessId(NPT_Integer& id) {
        return m_Delegate->GetProcessId(id);
    }
    NPT_Result GetCurrentTimeStamp(NPT_TimeStamp& now) {
        return m_Delegate->GetCurrentTimeStamp(now);
    }
    NPT_Result Sleep(const NPT_TimeInterval& duration) {
        return m_Delegate->Sleep(duration);
    }
    NPT_Result SleepUntil(const NPT_TimeStamp& when) {
        return m_Delegate->SleepUntil(when);
    }
    NPT_Result SetRandomSeed(unsigned int seed) {
        return m_Delegate->SetRandomSeed(seed);
    }
    NPT_Integer GetRandomInteger() {
        return m_Delegate->GetRandomInteger();
    }

 private:
    // static members
    static NPT_System System;

    // members
    NPT_SystemInterface* m_Delegate;
};

#endif // _NPT_SYSTEM_H_









