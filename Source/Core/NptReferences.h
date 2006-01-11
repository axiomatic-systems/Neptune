/*****************************************************************
|
|      Neptune - Interfaces
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
****************************************************************/

#ifndef _NPT_REFERENCES_H_
#define _NPT_REFERENCES_H_

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "NptConstants.h"

/*----------------------------------------------------------------------
|       NPT_Reference
+---------------------------------------------------------------------*/
template <typename T, typename C = NPT_Cardinal>
class NPT_Reference
{
public:
    // constructors and destructor
    NPT_Reference() : m_Object(NULL), m_Counter(NULL) {}
    explicit NPT_Reference(T* object) : 
        m_Object(object), 
        m_Counter(object?new C(1):NULL) {}
    NPT_Reference(const NPT_Reference<T>& ref) :
        m_Object(ref.m_Object), m_Counter(ref.m_Counter) {
        if (m_Counter) ++(*m_Counter);
    }
    ~NPT_Reference() {
        Release();
    }

    // overloaded operators
    NPT_Reference<T>& operator=(const NPT_Reference<T>& ref) {
        if (this != &ref) {
            Release();
            m_Object = ref.m_Object;
            m_Counter = ref.m_Counter;
            if (m_Counter) ++(*m_Counter);
        }
        return *this;
    }
    NPT_Reference<T>& operator=(T* object) {
        Release();
        m_Object = object;
        m_Counter = object?new C(1):NULL;
        return *this;
    }
    T& operator*() const { return *m_Object; }
    T* operator->() const { return m_Object; }

    // methods
    T* AsPointer() const { return m_Object; }
    bool IsNull() const { return m_Object == 0; }

private:
    // methods
    void Release() {
        if (m_Counter && --(*m_Counter) == 0) {
            delete m_Counter;
            delete m_Object;
        }
    }

    // members
    T* m_Object;
    C* m_Counter;
};

#endif // _NPT_REFERENCES_H_
