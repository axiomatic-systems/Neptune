/*****************************************************************
|
|      Threads Test Program 1
|
|      (c) 2001-2002 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "Neptune.h"

/*----------------------------------------------------------------------
|       Thread1
+---------------------------------------------------------------------*/
class Thread1 : public NPT_Thread
{
public:
    virtual ~Thread1() { NPT_Debug("~Thread1\n"); }
    void Run() {
        NPT_Debug("Thread1::Run - start\n");

        // sleep a while
        NPT_TimeInterval duration(1.2f);
        NPT_System::Sleep(duration);       

        NPT_Debug("Thread1::Run - end\n");
    }
};

/*----------------------------------------------------------------------
|       Thread2
+---------------------------------------------------------------------*/
class Thread2 : public NPT_Runnable
{
public:
    Thread2(NPT_SharedVariable* variable) : m_SharedVariable(variable) {}
    virtual ~Thread2() { NPT_Debug("~Thread2\n"); }
    void Run() {
        NPT_Debug("Thread2::Run - start\n");

        // sleep a while
        NPT_TimeInterval duration(2.1f);
        NPT_System::Sleep(duration);

        NPT_Debug("Thread2::Run - waiting for variable == 3\n");
        m_SharedVariable->WaitUntilEquals(3);
        NPT_Debug("Thread2::Run - end\n");
        NPT_Debug("Thread2::Run - deleting myself\n");
        delete this;
    }
    NPT_SharedVariable* m_SharedVariable;
};

/*----------------------------------------------------------------------
|       Thread3
+---------------------------------------------------------------------*/
class Thread3 : public NPT_Thread
{
public:
    Thread3(NPT_SharedVariable* variable) : NPT_Thread(false),
                                           m_SharedVariable(variable) {}
    virtual ~Thread3() { NPT_Debug("~Thread3\n"); }
    Thread3() : NPT_Thread(false) {}
    void Run() {
        NPT_Debug("Thread3::Run - start\n");

        // sleep a while
        NPT_TimeInterval duration(3.1f);
        NPT_System::Sleep(duration);

        NPT_Debug("Thread3::Run - setting shared var to 1\n");
        m_SharedVariable->SetValue(1);

         // sleep a while
        NPT_System::Sleep(duration);

        NPT_Debug("Thread3::Run - setting shared var to 2\n");
        m_SharedVariable->SetValue(2);

         // sleep a while
        NPT_System::Sleep(duration);      

        NPT_Debug("Thread3::Run - setting shared var to 3\n");
        m_SharedVariable->SetValue(3);
        NPT_Debug("Thread3::Run - end\n");
    }
    NPT_SharedVariable* m_SharedVariable;
};

/*----------------------------------------------------------------------
|       Thread4
+---------------------------------------------------------------------*/
class Thread4 : public NPT_Runnable
{
public:
    virtual ~Thread4() { NPT_Debug("~Thread4\n"); }
    void Run() {
        NPT_Debug("Thread4::Run - start\n");

        // sleep a while
        NPT_TimeInterval duration(4.3f);
        NPT_System::Sleep(duration);

        NPT_Debug("Thread4::Run - end\n");
    }
};

/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int
main(int argc, char** argv) 
{
    NPT_COMPILER_UNUSED(argc);
    NPT_COMPILER_UNUSED(argv);

    NPT_SharedVariable shv1(0);
    NPT_Thread* thread1 = new Thread1();
    Thread2 t2(&shv1);
    NPT_Thread* thread2 = new NPT_Thread(t2, true);
    Thread3 t3(&shv1);
    NPT_Thread* thread3 = new Thread3(t3);
    Thread4 t4;
    NPT_Thread* thread4 = new NPT_Thread(t4, false);

    NPT_Debug("starting thread1...\n");
    thread1->Start();

    NPT_Debug("starting thread2...\n");
    thread2->Start();

    NPT_Debug("starting thread3\n");
    thread3->Start();
    NPT_Debug("releasing thread3\n");
    delete thread3;

    NPT_Debug("starting thread4\n");
    thread4->Start();
    NPT_Debug("deleting thread4\n");
    delete thread4;

    NPT_Debug("deleting for thread1...\n");
    delete thread1;
    NPT_Debug("...done\n");
    
    // sleep a while
    NPT_TimeInterval duration(15UL);
    NPT_System::Sleep(duration);

    NPT_Debug("- program done -\n");

    return 0;
}






