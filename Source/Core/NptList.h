/*****************************************************************
|
|      Neptune - Lists
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

#ifndef _NPT_LIST_H_
#define _NPT_LIST_H_

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "NptResults.h"
#include "NptTypes.h"
#include "NptConstants.h"

/*----------------------------------------------------------------------
|       constants
+---------------------------------------------------------------------*/
const int NPT_ERROR_LIST_EMPTY             = NPT_ERROR_BASE_LIST - 0;
const int NPT_ERROR_LIST_OPERATION_ABORTED = NPT_ERROR_BASE_LIST - 1;

/*----------------------------------------------------------------------
|       NPT_List
+---------------------------------------------------------------------*/
template <typename T> 
class NPT_List 
{
public:
    // types
    class Item 
    {
     public:
        // types
        class Operator 
        {
        public:
            // methods
            virtual NPT_Result operator()(T& data) const = 0;
        };

        class Finder 
        {
        public:
            // methods
            virtual NPT_Result operator()(const T& data) const = 0;
        };

        // methods
        Item(const T& data) : m_Data(data), m_Next(0), m_Prev(0) {}
       ~Item() {}
        T&    operator()()    { return m_Data; }
        T&    GetData()       { return m_Data; }
        Item* GetNext() const { return m_Next; }
        Item* GetPrev() const { return m_Prev; }
    
     private:
        // members
        T     m_Data;
        Item* m_Next;
        Item* m_Prev;
        
        // friends
        friend class NPT_List<T>;
    };

    // convenience typdefs to help compilers who do not handle
    // the syntax NPT_List<T>::Item::Operator directly
    typedef typename Item::Operator ItemOperator;
    typedef typename Item::Finder   ItemFinder;

    // methods
                 NPT_List();
                 NPT_List(const NPT_List<T>& list);
                ~NPT_List();
    NPT_Result   Add(const T& data);
    NPT_Result   Add(Item& item);
    NPT_Result   Insert(Item* where, const T&data);
    NPT_Result   Insert(Item* where, Item& item);
    NPT_Result   Remove(T& data);
    NPT_Result   Remove(Item& item);
    NPT_Result   Detach(Item& item);
    NPT_Result   Get(NPT_Ordinal index, T& data) const;
    NPT_Result   Get(NPT_Ordinal index, T*& data) const;
    NPT_Result   PopHead(T& data);
    NPT_Result   Apply(const ItemOperator& op) const;
    NPT_Result   ApplyUntilFailure(const ItemOperator& op) const;
    NPT_Result   ApplyUntilSuccess(const ItemOperator& op) const;
    NPT_Result   ReverseApply(const ItemOperator& op) const;
    bool         Contains(const T& data) const;
    NPT_Result   Find(const ItemFinder& finder, 
                      T*& data, NPT_Ordinal index = 0) const;
    NPT_Result   Find(const ItemFinder& finder, 
                      T& data, NPT_Ordinal index = 0) const;
    NPT_Result   Find(const ItemFinder& finder, 
                      Item*& item, NPT_Ordinal index = 0) const;
    NPT_Result   ReverseFind(const ItemFinder& finder, 
                             T& data, NPT_Ordinal index = 0) const;
    NPT_Result   ReverseFind(const ItemFinder& finder, 
                             T*& data, NPT_Ordinal index = 0) const;
    NPT_Result   ReverseFind(ItemFinder& finder, 
                             Item*& item, 
                             NPT_Ordinal index = 0) const;
    NPT_Result   Empty();
    NPT_Result   DeleteReferences();
    NPT_Cardinal GetItemCount() const { return m_ItemCount; }
    Item*        GetFirstItem() const { return m_Head; }
    Item*        GetLastItem() const  { return m_Tail; }
 
    // operators
    void operator=(const NPT_List<T>& list);

private:
    // members
    NPT_Cardinal m_ItemCount;
    Item*        m_Head;
    Item*        m_Tail;
};

/*----------------------------------------------------------------------
|       NPT_List<T>::NPT_List
+---------------------------------------------------------------------*/
template <typename T>
inline
NPT_List<T>::NPT_List() : m_ItemCount(0), m_Head(0), m_Tail(0) 
{
}

/*----------------------------------------------------------------------
|       NPT_List<T>::NPT_List
+---------------------------------------------------------------------*/
template <typename T>
inline
NPT_List<T>::NPT_List(const NPT_List<T>& list)
{
    *this = list;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::~NPT_List<T>
+---------------------------------------------------------------------*/
template <typename T>
inline
NPT_List<T>::~NPT_List<T>()
{
    Empty();
}
 
/*----------------------------------------------------------------------
|       NPT_List<T>::operator=
+---------------------------------------------------------------------*/
template <typename T>
void
NPT_List<T>::operator=(const NPT_List<T>& list)
{
    // cleanup
    Empty();

    // copy the new list
    Item* item = list.GetFirstItem();
    while (item) {
        Add(item->GetData());
        item = item->GetNext();
    }
}

/*----------------------------------------------------------------------
|       NPT_List<T>::Empty
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::Empty()
{
    // delete all items
    Item* item = m_Head;
    while (item) {
        Item* next = item->m_Next;
        delete item;
        item = next;
    }

    m_ItemCount = 0;
    m_Head      = NULL;
    m_Tail      = NULL;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::DeleteReferences
+---------------------------------------------------------------------*/
template <typename T> 
NPT_Result
NPT_List<T>::DeleteReferences()
{
    Item* item = m_Head;

    while (item) {
        Item* next = item->m_Next;
        delete item->m_Data;
        delete item;
        item = next;
    }

    m_ItemCount = 0;
    m_Head      = NULL;
    m_Tail      = NULL;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::Add
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::Add(Item& item)
{
    // add element at the tail
    if (m_Tail) {
        item.m_Prev = m_Tail;
        item.m_Next = NULL;
        m_Tail->m_Next = &item;
        m_Tail = &item;
    } else {
        m_Head = &item;
        m_Tail = &item;
        item.m_Next = NULL;
        item.m_Prev = NULL;
    }

    // one more item in the list now
    ++m_ItemCount;
 
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::Add
+---------------------------------------------------------------------*/
template <typename T>
inline
NPT_Result
NPT_List<T>::Add(const T& data)
{
    return Add(*new Item(data));
}

/*----------------------------------------------------------------------
|       NPT_List<T>::Insert
+---------------------------------------------------------------------*/
template <typename T>
inline NPT_Result
NPT_List<T>::Insert(Item* where, const T& data)
{
    return Insert(where, new Item(data));
}

/*----------------------------------------------------------------------
|       NPT_List<T>::Insert
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::Insert(Item* where, Item& item)
{
    // insert the item in the list
    if (where == NULL) {
        // insert as the head
        if (m_Head) {
            // replace the current head
            item->m_Prev = NULL;
            item->m_Next = m_Head;
            m_Head->m_Prev = item;
            m_Head = item;
        } else {
            // this item becomes the head and tail
            m_Head = item;
            m_Tail = item;
            item->m_Next = NULL;
            item->m_Prev = NULL;
        }
    } else {
        // insert after the 'where' item
        if (where == m_Tail) {
            // add the item at the end
            return Add(item);
        } else {
            // update the links
            item->m_Prev = where;
            item->m_Next = where->m_Next;
            where->m_Next->m_Prev = item;
            where->m_Next = item;
        }
    }

    // one more item in the list now
    ++m_ItemCount;
 
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::Remove
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::Remove(Item& item)
{
    // detach the item
    NPT_RETURN_IF_FAILED(Detach(item));

    // delete the item
    delete item;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::Remove
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::Remove(T& data)
{
    Item* item = m_Head;

    while (item) {
        if (item->m_Data == data) {
            // remove item
            return Remove(item);
        }
        item = item->m_Next;
    }
 
    return NPT_ERROR_NO_SUCH_ITEM;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::Detach
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::Detach(Item& item)
{
    // remove item
    if (item.m_Prev) {
        // item is not the head
        if (item.m_Next) {
            // item is not the tail
            item.m_Next->m_Prev = item.m_Prev;
            item.m_Prev->m_Next = item.m_Next;
        } else {
            // item is the tail
            m_Tail = item.m_Prev;
            m_Tail->m_Next = NULL;
        }
    } else {
        // item is the head
        m_Head = item.m_Next;
        if (m_Head) {
            // item is not the tail
            m_Head->m_Prev = NULL;
        } else {
            // item is also the tail
            m_Tail = NULL;
        }
    }

    // one less item in the list now
    --m_ItemCount;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::Get
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::Get(NPT_Ordinal index, T& data) const
{
    Item* item = m_Head;

    if (index < m_ItemCount) {
        while (index--) item = item->m_Next;
        data = item->m_Data;
        return NPT_SUCCESS;
    } else {
        return NPT_ERROR_NO_SUCH_ITEM;
    }
}

/*----------------------------------------------------------------------
|       NPT_List<T>::Get
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::Get(NPT_Ordinal index, T*& data) const
{
    Item* item = m_Head;

    if (index < m_ItemCount) {
        while (index--) item = item->m_Next;
        data = &item->m_Data;
        return NPT_SUCCESS;
    } else {
        data = NULL;
        return NPT_ERROR_NO_SUCH_ITEM;
    }
}

/*----------------------------------------------------------------------
|       NPT_List<T>::PopHead
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::PopHead(T& data)
{
    // check that we have an element
    if (m_Head == NULL) return NPT_ERROR_LIST_EMPTY;

    // copy the head item's data
    data = m_Head->m_Data;

    // discard the head item
    Item* head = m_Head;
    m_Head = m_Head->m_Next;
    if (m_Head) {
        m_Head->m_Prev = NULL;
    } else {
        m_Tail = NULL;
    }
    delete head;

    // update the count
    --m_ItemCount;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::Apply
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::Apply(const ItemOperator& op) const
{
    Item* item = m_Head;
 
    while (item) {
        op(item->m_Data);
        item = item->m_Next;
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::ApplyUntilFailure
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::ApplyUntilFailure(const ItemOperator& op) const
{
    Item* item = m_Head;
 
    while (item) {
        NPT_RETURN_IF_FAILED(op(item->m_Data));
        item = item->m_Next;
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::ApplyUntilSuccess
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::ApplyUntilSuccess(const ItemOperator& op) const
{
    Item* item = m_Head;
 
    while (item) {
        NPT_Result result;
        result = op(item->m_Data);
        if (NPT_SUCCEEDED(result)) return NPT_SUCCESS;
        item = item->m_Next;
    }

    return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::ReverseApply
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::ReverseApply(const ItemOperator& op) const
{
    Item* item = m_Tail;
 
    while (item) {
        if (NPT_FAILED(op(item->m_Data))) {
            return NPT_ERROR_LIST_OPERATION_ABORTED;
        }
        item = item->m_Prev;
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::Contains
+---------------------------------------------------------------------*/
template <typename T>
bool
NPT_List<T>::Contains(const T& data) const
{
    Item* item = m_Head;
 
    while (item) {
        if (item->m_Data == data) {
            return true;
        }
        item = item->m_Next;
    }

    return false;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::Find
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::Find(const ItemFinder& finder, 
                  T&                data, 
                  NPT_Ordinal       index) const
{
    Item* item = m_Head;
 
    while (item) {
        if (NPT_SUCCEEDED(finder(item->m_Data)) && index-- == 0) {
            data = item->m_Data;
            return NPT_SUCCESS;
        }
        item = item->m_Next;
    }

    return NPT_ERROR_NO_SUCH_ITEM;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::Find
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::Find(const ItemFinder& finder, 
                  T*&               data,
                  NPT_Ordinal       index) const
{
    Item* item = m_Head;
 
    while (item) {
        if (NPT_SUCCEEDED(finder(item->m_Data)) && index-- == 0) {
            data = &item->m_Data;
            return NPT_SUCCESS;
        }
        item = item->m_Next;
    }

    data = NULL;
    return NPT_ERROR_NO_SUCH_ITEM;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::ReverseFind
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::ReverseFind(const ItemFinder& finder, 
                         T&                data,
                         NPT_Ordinal       index) const
{
    Item* item = m_Tail;
 
    while (item) {
        if (NPT_SUCCEEDED(finder(item->m_Data)) && index-- == 0) {
            data = item->m_Data;
            return NPT_SUCCESS;
        }
        item = item->m_Prev;
    }

    return NPT_ERROR_NO_SUCH_ITEM;
}

/*----------------------------------------------------------------------
|       NPT_List<T>::ReverseFind
+---------------------------------------------------------------------*/
template <typename T>
NPT_Result
NPT_List<T>::ReverseFind(const ItemFinder& finder, 
                         T*&               data,
                         NPT_Ordinal       index) const
{
    Item* item = m_Tail;
 
    while (item) {
        if (NPT_SUCCEEDED(finder(item->m_Data)) && index-- == 0) {
            data = &item->m_Data;
            return NPT_SUCCESS;
        }
        item = item->m_Prev;
    }

    data = NULL;
    return NPT_ERROR_NO_SUCH_ITEM;
}

/*----------------------------------------------------------------------
|       NPT_ObjectDeleter
+---------------------------------------------------------------------*/
template <class T> 
class NPT_ObjectDeleter : public NPT_List<T*>::ItemOperator 
{
public:
    // methods
    NPT_Result operator()(T*& data) const {
        delete data;
        return NPT_SUCCESS;
    }
};

#endif // _NPT_LIST_H_
