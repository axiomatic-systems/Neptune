/*****************************************************************
|
|      Lists Test Program 1
|
|      (c) 2005-2006 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Neptune.h"
#include "NptDebug.h"

/*----------------------------------------------------------------------
|       globals
+---------------------------------------------------------------------*/
static unsigned int A_Count = 0;

/*----------------------------------------------------------------------
|       types
+---------------------------------------------------------------------*/
class A {
public:
    A() : _a(0), _b(0), _c(&_a) {
        printf("A::A()\n");
        A_Count++;
    }
    A(int a, char b) : _a(a), _b(b), _c(&_a) {
        printf("A::A(%d, %d)\n", a, b);
        A_Count++;
    }
    A(const A& other) : _a(other._a), _b(other._b), _c(&_a) {
        printf("A::A(copy: a=%d, b=%d)\n", _a, _b);
        A_Count++;
    }
    ~A() {
        printf("A::~A(), a=%d, b=%d\n", _a, _b);
        A_Count--;
    }
    bool Check() { return _c == &_a; }
    bool operator==(const A& other) const {
        return _a == other._a && _b == other._b;
    }
    int _a;
    char _b;
    int* _c;
};

static int ApplyCounter = 0;
class Test1 {
public: 
    NPT_Result operator()(const A& a) const {
        ApplyCounter++;
        A aa(3,4);
        if (a == aa) return NPT_ERROR_OUT_OF_MEMORY;
        return NPT_SUCCESS;
    }
};

/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int
main(int /*argc*/, char** /*argv*/)
{
    NPT_List<A> a_list;
    a_list.Add(A(1,2));
    a_list.Add(A(2,3));
    a_list.Add(A(3,4));
    NPT_ASSERT(a_list.GetItemCount() == 3);
    NPT_ASSERT(a_list.Contains(A(2,3)));
    NPT_ASSERT(!a_list.Contains(A(7,8)));
    A a;
    NPT_ASSERT(NPT_SUCCEEDED(a_list.PopHead(a)));
    NPT_ASSERT(a == A(1,2));
    NPT_ASSERT(a_list.GetItemCount() == 2);
    NPT_ASSERT(NPT_SUCCEEDED(a_list.Get(0, a)));
    NPT_ASSERT(a == A(2,3));
    A* pa = NULL;
    NPT_ASSERT(NPT_SUCCEEDED(a_list.Get(0,pa)));
    NPT_ASSERT(pa != NULL);
    NPT_ASSERT(*pa == A(2,3));
    NPT_ASSERT(a_list.GetItem(1) == ++a_list.GetFirstItem());

    a_list.Clear();
    NPT_ASSERT(a_list.GetItemCount() == 0);
    a_list.Insert(a_list.GetFirstItem(), A(7,9));
    NPT_ASSERT(a_list.GetItemCount() == 1);
    NPT_ASSERT(*a_list.GetFirstItem() == A(7,9));

    a_list.Add(A(1, 2));
    NPT_ASSERT(a_list.GetItemCount() == 2);
    NPT_ASSERT(A_Count == 3);
    NPT_ASSERT(*a_list.GetFirstItem() == A(7,9));
    NPT_ASSERT(*a_list.GetLastItem()  == A(1,2));
    
    a_list.Insert(a_list.GetLastItem(), A(3,4));
    NPT_ASSERT(a_list.GetItemCount() == 3);
    NPT_ASSERT(*a_list.GetLastItem() == A(1,2));

    // test ApplyUntil 
    ApplyCounter = 0;
    bool applied;
    NPT_Result result = a_list.ApplyUntil(Test1(), NPT_UntilResultEquals(NPT_ERROR_OUT_OF_MEMORY), &applied);
    NPT_ASSERT(applied == true);
    NPT_ASSERT(result == NPT_SUCCESS);
    NPT_ASSERT(ApplyCounter == 2);

    ApplyCounter = 0;
    result = a_list.ApplyUntil(Test1(), NPT_UntilResultNotEquals(NPT_SUCCESS), &applied);
    NPT_ASSERT(applied == true);
    NPT_ASSERT(result == NPT_ERROR_OUT_OF_MEMORY);
    NPT_ASSERT(ApplyCounter == 2);

    ApplyCounter = 0;
    result = a_list.ApplyUntil(Test1(), NPT_UntilResultEquals(NPT_FAILURE), &applied);
    NPT_ASSERT(applied == false);
    NPT_ASSERT(result == NPT_SUCCESS);
    NPT_ASSERT(ApplyCounter == 3);

    a_list.Insert(NPT_List<A>::Iterator(NULL), A(3,4));
    NPT_ASSERT(a_list.GetItemCount() == 4);
    NPT_ASSERT(*a_list.GetLastItem() == A(3,4));

    a_list.Insert(a_list.GetFirstItem(), A(7,8));
    NPT_ASSERT(a_list.GetItemCount() == 5);
    NPT_ASSERT(*a_list.GetFirstItem() == A(7,8));

    a_list.Insert(a_list.GetItem(2), A(9,10));
    NPT_ASSERT(a_list.GetItemCount() == 6);
    NPT_ASSERT(*a_list.GetItem(2) == A(9,10));

    a_list.Erase(a_list.GetItem(1));
    NPT_ASSERT(a_list.GetItemCount() == 5);
    NPT_ASSERT(*a_list.GetItem(1) == A(9,10));
    NPT_ASSERT(A_Count == 1+a_list.GetItemCount());

    NPT_List<int> i1_list;
    NPT_List<int> i2_list;
    NPT_ASSERT(i1_list == i2_list);
    i1_list.Add(3);
    NPT_ASSERT(i1_list != i2_list);
    NPT_ASSERT(!(i1_list == i2_list));
    i2_list.Add(3);
    NPT_ASSERT(i1_list == i2_list);
    i2_list.Add(4);
    NPT_ASSERT(i1_list != i2_list);
    i1_list.Add(4);
    i1_list.Add(5);
    i2_list.Add(6);
    NPT_ASSERT(i1_list != i2_list);
  

    // NPT_Stack test
    NPT_Stack<int> i_stack;
    int i=0;
    NPT_ASSERT(NPT_FAILED(i_stack.Pop(i)));
    NPT_ASSERT(NPT_FAILED(i_stack.Peek(i)));
    NPT_ASSERT(NPT_SUCCEEDED(i_stack.Push(4)));
    NPT_ASSERT(NPT_SUCCEEDED(i_stack.Push(5)));
    NPT_ASSERT(NPT_SUCCEEDED(i_stack.Push(6)));
    NPT_ASSERT(NPT_SUCCEEDED(i_stack.Pop(i)));
    NPT_ASSERT(i == 6);
    NPT_ASSERT(NPT_SUCCEEDED(i_stack.Peek(i)));
    NPT_ASSERT(i == 5);
    NPT_ASSERT(NPT_SUCCEEDED(i_stack.Pop(i)));
    NPT_ASSERT(i == 5);
    NPT_ASSERT(NPT_SUCCEEDED(i_stack.Pop(i)));
    NPT_ASSERT(i == 4);

    return 0;
}
