/*****************************************************************
|
|      Arrays Test Program 1
|
|      (c) 2005 Gilles Boccon-Gibod
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
    bool operator==(const A& other) {
        return _a == other._a && _b == other._b;
    }
    int _a;
    char _b;
    int* _c;
};

/*----------------------------------------------------------------------
|       Fail
+---------------------------------------------------------------------*/
static void
Fail()
{
    printf("FAIL ##################################\n");
    exit(1);
}

/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int
main(int /*argc*/, char** /*argv*/)
{
    NPT_Array<int> a;
    a.Append(7);
    NPT_ASSERT(a[0] == 7);

    NPT_Array<A> a_array;
    a_array.Append(A(1,2));
    a_array.Append(A(3,4));
    a_array.Reserve(100);
    a_array.Append(A(4,5));

    NPT_ASSERT(A_Count == 3);
    NPT_Array<A> b_array = a_array;
    NPT_ASSERT(A_Count == 6);
    NPT_ASSERT(b_array.GetItemCount() == a_array.GetItemCount());
    NPT_ASSERT(b_array == a_array);
    NPT_ASSERT(a_array[0] == b_array[0]);
    b_array[0] = A(7,8);
    NPT_ASSERT(A_Count == 6);
    NPT_ASSERT(!(a_array[0] == b_array[0]));

    a_array.Resize(2);
    NPT_ASSERT(A_Count == 5);
    NPT_ASSERT(a_array.GetItemCount() == 2);
    b_array.Resize(5);
    NPT_ASSERT(A_Count == 7);
    NPT_ASSERT(b_array[4]._a == 0);
    NPT_ASSERT(b_array[4]._b == 0);
    
    a_array.Resize(6, A(9,10));
    NPT_ASSERT(A_Count == 11);
    NPT_ASSERT(a_array[5] == A(9,10));

    for (NPT_Ordinal i=0; i<a_array.GetItemCount(); i++) {
        a_array[i].Check();
    }
    for (NPT_Ordinal i=0; i<b_array.GetItemCount(); i++) {
        b_array[i].Check();
    }

    NPT_Array<int>* int_array = new NPT_Array<int>(100);
    NPT_ASSERT(int_array->GetItemCount() == 0);
    int_array->Append(1);
    int_array->Append(2);
    NPT_ASSERT(int_array->GetItemCount() == 2);
    NPT_ASSERT((*int_array)[0] == 1);
    NPT_ASSERT((*int_array)[1] == 2);
    int_array->Clear();
    NPT_ASSERT(int_array->GetItemCount() == 0);
    delete int_array;

}
