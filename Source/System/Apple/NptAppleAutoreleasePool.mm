/*****************************************************************
 |
 |      Neptune - Autorelease Pool :: Apple Implementation
 |
 |      Author: Edin Hodzic (dino@concisoft.com)
 |
 ****************************************************************/


/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include <Foundation/Foundation.h>
#include "NptAutoreleasePool.h"

/*----------------------------------------------------------------------
|   AppleAutoReleasePool
+---------------------------------------------------------------------*/
class AppleAutoreleasePool : public NPT_AutoreleasePoolInterface 
{
public:
    AppleAutoreleasePool();
    virtual ~AppleAutoreleasePool();

private:
    NSAutoreleasePool* m_Pool;
};

/*----------------------------------------------------------------------
|   AppleAutoreleasePool::AppleAutoreleasePool
+---------------------------------------------------------------------*/
AppleAutoreleasePool::AppleAutoreleasePool() 
{
    m_Pool = [[NSAutoreleasePool alloc] init];
}

/*----------------------------------------------------------------------
|   AppleAutoreleasePool::~AppleAutoreleasePool
+---------------------------------------------------------------------*/
AppleAutoreleasePool::~AppleAutoreleasePool() 
{
    [m_Pool drain];
    m_Pool = NULL;
}

/*----------------------------------------------------------------------
|   NPT_AutoreleasePool::NPT_AutoreleasePool
+---------------------------------------------------------------------*/
NPT_AutoreleasePool::NPT_AutoreleasePool()
{
    m_Delegate = new AppleAutoreleasePool;
}

/*----------------------------------------------------------------------
|   NPT_AutoreleasePool::~NPT_AutoreleasePool
+---------------------------------------------------------------------*/
NPT_AutoreleasePool::~NPT_AutoreleasePool()
{
    delete m_Delegate;
    m_Delegate = NULL;
}
