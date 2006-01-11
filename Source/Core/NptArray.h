/*****************************************************************
|
|      Neptune - Arrays
|
|      (c) 2001-2005 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
****************************************************************/

#ifndef _NPT_ARRAY_H_
#define _NPT_ARRAY_H_

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <new>
#include "NptTypes.h"
#include "NptResults.h"

/*----------------------------------------------------------------------
|       constants
+---------------------------------------------------------------------*/
const int NPT_ARRAY_INITIAL_MAX_SIZE = 1024;

/*----------------------------------------------------------------------
|       NPT_Array
+---------------------------------------------------------------------*/
template <typename T> 
class NPT_Array 
{
public:
    // methods
    NPT_Array<T>(): m_Capacity(0), m_ItemCount(0), m_Items(0) {}
    explicit NPT_Array<T>(NPT_Cardinal count);
    NPT_Array<T>(NPT_Cardinal count, const T& item);
   ~NPT_Array<T>();
    NPT_Array<T>(const NPT_Array<T>& copy);
    NPT_Array<T>& operator=(const NPT_Array<T>& copy);
    bool          operator==(const NPT_Array<T>& other) const;
    bool          operator!=(const NPT_Array<T>& other) const;
    NPT_Cardinal GetItemCount() const { return m_ItemCount; }
    NPT_Result   Append(const T& item);
    T& operator[](NPT_Ordinal pos)             { return m_Items[pos]; }
    const T& operator[](NPT_Ordinal pos) const { return m_Items[pos]; }
    NPT_Result   Erase(NPT_Ordinal pos);
    NPT_Result   Erase(NPT_Ordinal first, NPT_Ordinal last);
    NPT_Result   Insert(NPT_Ordinal pos, const T& item, NPT_Cardinal count = 1);
    NPT_Result   Reserve(NPT_Cardinal count);
    NPT_Cardinal GetCapacity(NPT_Cardinal count) const;
    NPT_Result   Resize(NPT_Cardinal count);
    NPT_Result   Resize(NPT_Cardinal count, const T& fill);
    NPT_Result   Clear();

protected:
    // members
    NPT_Cardinal m_Capacity;
    NPT_Cardinal m_ItemCount;
    T*           m_Items;
};

/*----------------------------------------------------------------------
|       NPT_Array<T>::NPT_Array<T>
+---------------------------------------------------------------------*/
template <typename T>
NPT_Array<T>::NPT_Array<T>(NPT_Cardinal count) :
    m_Capacity(0),
    m_ItemCount(0),
    m_Items(0)
{
    Reserve(count);
}

/*----------------------------------------------------------------------
|       NPT_Array<T>::NPT_Array<T>
+---------------------------------------------------------------------*/
template <typename T>
NPT_Array<T>::NPT_Array<T>(const NPT_Array<T>& copy) :
    m_Capacity(0),
    m_ItemCount(0),
    m_Items(0)
{
    Reserve(copy.GetItemCount());
    for (NPT_Ordinal i=0; i<copy.m_ItemCount; i++) {
        new ((void*)&m_Items[i]) T(copy.m_Items[i]);
    }
    m_ItemCount = copy.m_ItemCount;
}

/*----------------------------------------------------------------------
|       NPT_Array<T>::NPT_Array<T>
+---------------------------------------------------------------------*/
template <typename T>
NPT_Array<T>::NPT_Array<T>(NPT_Cardinal count, const T& item) :
    m_Capacity(0),
    m_ItemCount(0),
    m_Items(0)    
{
    Reserve(count);
    for (NPT_Ordinal i=0; i<count; i++) {
        new ((void*)&m_Items[i]) T(item);
    }
    m_ItemCount = count;
}

/*----------------------------------------------------------------------
|       NPT_Array<T>::~NPT_Array<T>
+---------------------------------------------------------------------*/
template <typename T>
NPT_Array<T>::~NPT_Array<T>()
{
    // remove all items
    Clear();

    // free the memory
    ::operator delete((void*)m_Items);
}

/*----------------------------------------------------------------------
|       NPT_Array<T>::operator=
+---------------------------------------------------------------------*/
template <typename T>
NPT_Array<T>&
NPT_Array<T>::operator=(const NPT_Array<T>& copy)
{
    // do nothing if we're assigning to ourselves
    if (this == &copy) return copy;

    // destroy all elements
    Clear();

    // copy all elements from the other object
    Reserve(copy.GetItemCount());
    for (NPT_Ordinal i=0; i<copy.m_ItemCount; i++) {
        new ((void*)&m_Items[i]) T(copy.m_Item[i]);
    }

    return *this;
}

/*----------------------------------------------------------------------
|       NPT_Array<T>::Clear
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_Array<T>::Clear()
{
    // destroy all items
    for (NPT_Ordinal i=0; i<m_ItemCount; i++) {
        m_Items[i].~T();
    }

    m_ItemCount = 0;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Array<T>::Reserve
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_Array<T>::Reserve(NPT_Cardinal count)
{
    if (count <= m_Capacity) return NPT_SUCCESS;

    NPT_Cardinal new_capacity;
    if (m_Capacity) {
        new_capacity = 2*m_Capacity;
    } else {
        // start with just enough elements to fill 
        // NPT_ARRAY_INITIAL_MAX_SIZE worth of memory
        new_capacity = NPT_ARRAY_INITIAL_MAX_SIZE/sizeof(T);
        if (new_capacity == 0) new_capacity = 1;
    }

    // (re)allocate the items
    T* new_items = (T*)::operator new(new_capacity*sizeof(T));
    if (new_items == NULL) {
        return NPT_ERROR_OUT_OF_MEMORY;
    }
    if (m_ItemCount && m_Items) {
        for (unsigned int i=0; i<m_ItemCount; i++) {
            // construct the copy
            new ((void*)&new_items[i])T(m_Items[i]);

            // destroy the item
            m_Items[i].~T();
        }
        ::operator delete((void*)m_Items);;
    }
    m_Items = new_items;
    m_Capacity = new_capacity;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Array<T>::Append
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_Array<T>::Append(const T& item)
{
    // ensure capacity
    NPT_Result result = Reserve(m_ItemCount+1);
    if (result != NPT_SUCCESS) return result;

    // store the item
    new ((void*)&m_Items[m_ItemCount++]) T(item);

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Array<T>::Resize
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_Array<T>::Resize(NPT_Cardinal size)
{
    if (size < m_ItemCount) {
        // shrink
        for (NPT_Ordinal i=size; i<m_ItemCount; i++) {
            m_Items[i].~T();
        }
        m_ItemCount = size;
    } else if (size > m_ItemCount) {
        return Resize(size, T());
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Array<T>::Resize
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_Array<T>::Resize(NPT_Cardinal size, const T& fill)
{
    if (size < m_ItemCount) {
        return Resize(size);
    } else if (size > m_ItemCount) {
        for (NPT_Ordinal i=m_ItemCount; i<size; i++) {
            new ((void*)&m_Items[i]) T(fill);
        }
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_Array<T>::operator==
+---------------------------------------------------------------------*/
template <typename T>
bool
NPT_Array<T>::operator==(const NPT_Array<T>& other) const
{
    // we need the same number of items
    if (other.m_ItemCount != m_ItemCount) return false;

    // compare all items
    for (NPT_Ordinal i=0; i<m_ItemCount; i++) {
        if (!(m_Items[i] == other.m_Items[i])) return false;
    }

    return true;
}

/*----------------------------------------------------------------------
|       NPT_Array<T>::operator!=
+---------------------------------------------------------------------*/
template <typename T>
bool
NPT_Array<T>::operator!=(const NPT_Array<T>& other) const
{
    return !(*this == other);
}

#endif // _NPT_ARRAY_H_













