/*****************************************************************
|
|      Neptune - Threads :: Win32 Implementation
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <windows.h>
#include <process.h>

#include "NptConfig.h"
#include "NptTypes.h"
#include "NptConstants.h"
#include "NptThreads.h"
#include "NptDebug.h"
#include "NptResults.h"
#include "NptWin32Threads.h"

/*----------------------------------------------------------------------
|       configuration macros
+---------------------------------------------------------------------*/
#define NPT_WIN32_USE_CREATE_THREAD
#if defined(NPT_WIN32_USE_CREATE_THREAD)
#define _beginthreadex(security, stack_size, start_proc, arg, flags,pid) \
CreateThread(security, stack_size, (LPTHREAD_START_ROUTINE) start_proc,  \
             arg, flags, pid)
#define _endthreadex ExitThread
#endif

/*----------------------------------------------------------------------
|       NPT_Win32Mutex::NPT_Win32Mutex
+---------------------------------------------------------------------*/
NPT_Win32Mutex::NPT_Win32Mutex()
{
    m_Handle = CreateMutex(NULL, FALSE, NULL);
}

/*----------------------------------------------------------------------
|       NPT_Win32Mutex::~NPT_Win32Mutex
+---------------------------------------------------------------------*/
NPT_Win32Mutex::~NPT_Win32Mutex()
{
    CloseHandle(m_Handle);
}

/*----------------------------------------------------------------------
|       NPT_Win32Mutex::Lock
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32Mutex::Lock()
{
    DWORD result = WaitForSingleObject(m_Handle, INFINITE);
    if (result == WAIT_OBJECT_0) {
        return NPT_SUCCESS;
    } else {
        return NPT_FAILURE;
    }
}

/*----------------------------------------------------------------------
|       NPT_Win32Mutex::Unlock
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32Mutex::Unlock()
{
    ReleaseMutex(m_Handle);
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Mutex::NPT_Mutex
+---------------------------------------------------------------------*/
NPT_Mutex::NPT_Mutex()
{
    m_Delegate = new NPT_Win32Mutex();
}

/*----------------------------------------------------------------------
|       NPT_Win32CriticalSection::NPT_Win32CriticalSection
+---------------------------------------------------------------------*/
NPT_Win32CriticalSection::NPT_Win32CriticalSection()
{
    InitializeCriticalSection(&m_CriticalSection);
}

/*----------------------------------------------------------------------
|       NPT_Win32CriticalSection::~NPT_Win32CriticalSection
+---------------------------------------------------------------------*/
NPT_Win32CriticalSection::~NPT_Win32CriticalSection()
{
    DeleteCriticalSection(&m_CriticalSection);
}

/*----------------------------------------------------------------------
|       NPT_Win32CriticalSection::Lock
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32CriticalSection::Lock()
{
    EnterCriticalSection(&m_CriticalSection);
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Win32CriticalSection::Unlock
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32CriticalSection::Unlock()
{
    LeaveCriticalSection(&m_CriticalSection);
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Win32Event
+---------------------------------------------------------------------*/
class NPT_Win32Event
{
public:
    // methods
    NPT_Win32Event(bool bManual = false);
    virtual ~NPT_Win32Event();

    // NPT_Mutex methods
    virtual NPT_Result Wait(NPT_Timeout timeout = NPT_TIMEOUT_INFINITE);
    virtual void       Signal();
    virtual void       Reset();

private:
    // members
    HANDLE m_Event;
};

/*----------------------------------------------------------------------
|       NPT_Win32Event::NPT_Win32Event
+---------------------------------------------------------------------*/
NPT_Win32Event::NPT_Win32Event(bool bManual)
{
    m_Event = CreateEvent(NULL, (bManual==true)?TRUE:FALSE, FALSE, NULL);
}

/*----------------------------------------------------------------------
|       NPT_Win32Event::~NPT_Win32Event
+---------------------------------------------------------------------*/
NPT_Win32Event::~NPT_Win32Event()
{
    CloseHandle(m_Event);
}

/*----------------------------------------------------------------------
|       NPT_Win32Event::Wait
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32Event::Wait(NPT_Timeout timeout)
{
    if (m_Event) {
        DWORD result = WaitForSingleObject(m_Event, timeout==NPT_TIMEOUT_INFINITE?INFINITE:timeout);
        if (result == WAIT_TIMEOUT) {
            return NPT_ERROR_TIMEOUT;
        }
        if (result != WAIT_OBJECT_0 && result != WAIT_ABANDONED) {
            return NPT_FAILURE;
        }
    }
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Win32Event::Signal
+---------------------------------------------------------------------*/
void
NPT_Win32Event::Signal()
{
    SetEvent(m_Event);
}

/*----------------------------------------------------------------------
|       NPT_Win32Event::Reset
+---------------------------------------------------------------------*/
void
NPT_Win32Event::Reset()
{
    ResetEvent(m_Event);
}

/*----------------------------------------------------------------------
|       NPT_Win32SharedVariable
+---------------------------------------------------------------------*/
class NPT_Win32SharedVariable : public NPT_SharedVariableInterface
{
 public:
    // methods
               NPT_Win32SharedVariable(NPT_Integer value);
              ~NPT_Win32SharedVariable();
    NPT_Result SetValue(NPT_Integer value);
    NPT_Result GetValue(NPT_Integer& value);
    NPT_Result WaitUntilEquals(NPT_Integer value, NPT_Timeout timeout = NPT_TIMEOUT_INFINITE);
    NPT_Result WaitWhileEquals(NPT_Integer value, NPT_Timeout timeout = NPT_TIMEOUT_INFINITE);

 private:
    // members
    volatile NPT_Integer m_Value;
    NPT_Mutex            m_Lock;
    NPT_Win32Event       m_Event;
};

/*----------------------------------------------------------------------
|       NPT_Win32SharedVariable::NPT_Win32SharedVariable
+---------------------------------------------------------------------*/
NPT_Win32SharedVariable::NPT_Win32SharedVariable(NPT_Integer value) : 
    m_Value(value)
{
}

/*----------------------------------------------------------------------
|       NPT_Win32SharedVariable::~NPT_Win32SharedVariable
+---------------------------------------------------------------------*/
NPT_Win32SharedVariable::~NPT_Win32SharedVariable()
{
}

/*----------------------------------------------------------------------
|       NPT_Win32SharedVariable::SetValue
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32SharedVariable::SetValue(NPT_Integer value)
{
    m_Lock.Lock();
    m_Value = value;
    m_Event.Signal();
    m_Lock.Unlock();
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Win32SharedVariable::GetValue
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32SharedVariable::GetValue(NPT_Integer& value)
{
    // reading an integer should be atomic on most platforms
    value = m_Value;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Win32SharedVariable::WaitUntilEquals
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32SharedVariable::WaitUntilEquals(NPT_Integer value, NPT_Timeout timeout)
{
    do {
        m_Lock.Lock();
        if (m_Value == value) {
            break;
        }
        m_Event.Reset();
        m_Lock.Unlock();
        if (NPT_FAILED(m_Event.Wait(timeout))) {
            return NPT_FAILURE;
        }
    } while (1);

    m_Lock.Unlock();
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Win32SharedVariable::WaitWhileEquals
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32SharedVariable::WaitWhileEquals(NPT_Integer value, NPT_Timeout timeout)
{
    do {
        m_Lock.Lock();
        if (m_Value != value) {
            break;
        }
        m_Event.Reset();
        m_Lock.Unlock();
        if (NPT_FAILED(m_Event.Wait(timeout))) {
            return NPT_FAILURE;
        }
    } while (1);

    m_Lock.Unlock();
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_SharedVariable::NPT_SharedVariable
+---------------------------------------------------------------------*/
NPT_SharedVariable::NPT_SharedVariable(NPT_Integer value)
{
    m_Delegate = new NPT_Win32SharedVariable(value);
}

/*----------------------------------------------------------------------
|       NPT_Win32AtomicVariable
+---------------------------------------------------------------------*/
class NPT_Win32AtomicVariable : public NPT_AtomicVariableInterface
{
 public:
    // methods
                NPT_Win32AtomicVariable(NPT_Integer value);
               ~NPT_Win32AtomicVariable();
    NPT_Integer Increment(); 
    NPT_Integer Decrement();
    void        SetValue(NPT_Integer value);
    NPT_Integer GetValue();

 private:
    // members
    volatile LONG m_Value;
};

/*----------------------------------------------------------------------
|       NPT_Win32AtomicVariable::NPT_Win32AtomicVariable
+---------------------------------------------------------------------*/
NPT_Win32AtomicVariable::NPT_Win32AtomicVariable(NPT_Integer value) : 
    m_Value(value)
{
}

/*----------------------------------------------------------------------
|       NPT_Win32AtomicVariable::~NPT_Win32AtomicVariable
+---------------------------------------------------------------------*/
NPT_Win32AtomicVariable::~NPT_Win32AtomicVariable()
{
}

/*----------------------------------------------------------------------
|       NPT_Win32AtomicVariable::Increment
+---------------------------------------------------------------------*/
NPT_Integer
NPT_Win32AtomicVariable::Increment()
{
    return InterlockedIncrement(const_cast<LONG*>(&m_Value));
}

/*----------------------------------------------------------------------
|       NPT_Win32AtomicVariable::Decrement
+---------------------------------------------------------------------*/
NPT_Integer
NPT_Win32AtomicVariable::Decrement()
{
    return InterlockedDecrement(const_cast<LONG*>(&m_Value));
}

/*----------------------------------------------------------------------
|       NPT_Win32AtomicVariable::SetValue
+---------------------------------------------------------------------*/
void
NPT_Win32AtomicVariable::SetValue(NPT_Integer value)
{
    m_Value = value;
}

/*----------------------------------------------------------------------
|       NPT_Win32AtomicVariable::GetValue
+---------------------------------------------------------------------*/
NPT_Integer
NPT_Win32AtomicVariable::GetValue()
{
    return m_Value;
}

/*----------------------------------------------------------------------
|       NPT_AtomicVariable::NPT_AtomicVariable
+---------------------------------------------------------------------*/
NPT_AtomicVariable::NPT_AtomicVariable(NPT_Integer value)
{
    m_Delegate = new NPT_Win32AtomicVariable(value);
}

/*----------------------------------------------------------------------
|       NPT_Win32Thread
+---------------------------------------------------------------------*/
class NPT_Win32Thread : public NPT_ThreadInterface
{
 public:
    // methods
                NPT_Win32Thread(NPT_Thread*   delegator,
                                NPT_Runnable& target,
                                bool          detached);
               ~NPT_Win32Thread();
    NPT_Result  Start(); 
    NPT_Result  Wait();
    NPT_Result  Terminate();

 private:
    // methods
    static unsigned int __stdcall EntryPoint(void* argument);

    // NPT_Runnable methods
    void Run();

    // members
    NPT_Thread*   m_Delegator;
    NPT_Runnable& m_Target;
    bool          m_Detached;
    HANDLE        m_ThreadHandle;
    DWORD         m_ThreadId;
};

/*----------------------------------------------------------------------
|       NPT_Win32Thread::NPT_Win32Thread
+---------------------------------------------------------------------*/
NPT_Win32Thread::NPT_Win32Thread(NPT_Thread*   delegator,
                                 NPT_Runnable& target,
                                 bool          detached) : 
    m_Delegator(delegator),
    m_Target(target),
    m_Detached(detached),
    m_ThreadHandle(0),
    m_ThreadId(0)
{
}

/*----------------------------------------------------------------------
|       NPT_Win32Thread::~NPT_Win32Thread
+---------------------------------------------------------------------*/
NPT_Win32Thread::~NPT_Win32Thread()
{
    if (!m_Detached) {
        // we're not detached, and not in the Run() method, so we need to 
        // wait until the thread is done
        Wait();
    }

    // close the thread handle
    CloseHandle(m_ThreadHandle);
}

/*----------------------------------------------------------------------
|       NPT_Win32Thread::Terminate
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32Thread::Terminate()
{
    // end the thread
    _endthreadex(0);

    // if we're detached, we need to delete ourselves
    if (m_Detached) {
        delete m_Delegator;
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Win32Thread::EntryPoint
+---------------------------------------------------------------------*/
unsigned int __stdcall
NPT_Win32Thread::EntryPoint(void* argument)
{
    NPT_Win32Thread* thread = reinterpret_cast<NPT_Win32Thread*>(argument);

    NPT_Debug(":: NPT_Win32Thread::EntryPoint - in =======================\n");

    // run the thread 
    thread->Run();
    
    NPT_Debug(":: NPT_Win32Thread::EntryPoint - out ======================\n");

    // we're done with the thread object
    thread->Terminate();

    // done
    return 0;
}

/*----------------------------------------------------------------------
|       NPT_Win32Thread::Start
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32Thread::Start()
{
    if (m_ThreadHandle > 0) {
        // failed
        NPT_Debug(":: NPT_Win32Thread::Start already started !\n");
        return NPT_ERROR_INVALID_STATE;
    }

    NPT_Debug(":: NPT_Win32Thread::Start - creating thread\n");

    // create the native thread
    m_ThreadHandle = (HANDLE)
        _beginthreadex(NULL, 
                       0, 
                       EntryPoint, 
                       reinterpret_cast<void*>(this), 
                       0, 
                       &m_ThreadId);
    if (m_ThreadHandle == 0) {
        // failed
        return NPT_FAILURE;
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Win32Thread::Run
+---------------------------------------------------------------------*/
void
NPT_Win32Thread::Run()
{
    m_Target.Run();
}

/*----------------------------------------------------------------------
|       NPT_Win32Thread::Wait
+---------------------------------------------------------------------*/
NPT_Result
NPT_Win32Thread::Wait()
{
    // check that we're not detached
    if (m_ThreadHandle == 0 || m_Detached) {
        return NPT_FAILURE;
    }

    // wait for the thread to finish
    NPT_Debug(":: NPT_Win32Thread::Wait - joining thread id %d\n", m_ThreadId);
    DWORD result = WaitForSingleObject(m_ThreadHandle, INFINITE);
    if (result != WAIT_OBJECT_0) {
        return NPT_FAILURE;
    } else {
        return NPT_SUCCESS;
    }
}

/*----------------------------------------------------------------------
|       NPT_Thread::NPT_Thread
+---------------------------------------------------------------------*/
NPT_Thread::NPT_Thread(bool detached)
{
    m_Delegate = new NPT_Win32Thread(this, *this, detached);
}

/*----------------------------------------------------------------------
|       NPT_Thread::NPT_Thread
+---------------------------------------------------------------------*/
NPT_Thread::NPT_Thread(NPT_Runnable& target, bool detached)
{
    m_Delegate = new NPT_Win32Thread(this, target, detached);
}












