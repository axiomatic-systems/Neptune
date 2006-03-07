/*****************************************************************
|
|      Maps Test Program 1
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
        A_Count++;
    }
    A(const A& other) : _a(other._a), _b(other._b), _c(&_a) {
        A_Count++;
    }
    ~A() {
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

/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int
main(int /*argc*/, char** /*argv*/)
{
    NPT_Map<NPT_String,A> a_map;
    A* a = NULL;

    NPT_ASSERT(a_map.GetEntryCount() == 0);
    NPT_ASSERT(a_map.HasKey("hello") == false);
    NPT_ASSERT(!a_map.HasValue(A(1,2)));
    NPT_ASSERT(NPT_FAILED(a_map.Get("bla", a)));
    NPT_ASSERT(a == NULL);

    a_map.Put("hello", A(1,2));
    NPT_ASSERT(a_map.GetEntryCount() == 1);
    NPT_ASSERT(NPT_SUCCEEDED(a_map.Get("hello", a)));
    NPT_ASSERT(*a == A(1,2));
    NPT_ASSERT(a_map.HasKey("hello"));
    NPT_ASSERT(a_map.HasValue(A(1,2)));
    NPT_ASSERT(a_map["hello"] == A(1,2));
    
    NPT_ASSERT(a_map["bla"] == A());
    NPT_ASSERT(a_map.GetEntryCount() == 2);
    a_map["bla"] = A(3,4);
    NPT_ASSERT(a_map["bla"] == A(3,4));
    NPT_ASSERT(a_map.GetEntryCount() == 2);

    NPT_Map<NPT_String,A> b_map;
    b_map["hello"] = A(1,2);
    b_map["bla"] = A(3,4);
    NPT_ASSERT(a_map == b_map);

    NPT_Map<NPT_String,A> c_map = a_map;
    NPT_ASSERT(c_map["hello"] == a_map["hello"]);
    NPT_ASSERT(c_map["bla"] == a_map["bla"]);

    NPT_ASSERT(NPT_SUCCEEDED(a_map.Put("bla", A(5,6))));
    NPT_ASSERT(NPT_SUCCEEDED(a_map.Get("bla", a)));
    NPT_ASSERT(*a == A(5,6));
    NPT_ASSERT(NPT_FAILED(a_map.Get("youyou", a)));

    b_map.Clear();
    NPT_ASSERT(b_map.GetEntryCount() == 0);

    a_map["youyou"] = A(6,7);
    NPT_ASSERT(NPT_FAILED(a_map.Erase("coucou")));
    NPT_ASSERT(NPT_SUCCEEDED(a_map.Erase("bla")));
    NPT_ASSERT(!a_map.HasKey("bla"));

    NPT_ASSERT(!(a_map == c_map));
    NPT_ASSERT(c_map != a_map);

    c_map = a_map;
    NPT_Map<NPT_String,A> d_map(c_map);
    NPT_ASSERT(d_map == c_map);

    NPT_Map<int,int> i_map;
    i_map[5] = 6;
    i_map[6] = 7;
    i_map[9] = 0;
    NPT_ASSERT(i_map[0] == 0 || i_map[0] != 0); // unknown value
    NPT_ASSERT(i_map.GetEntryCount() == 4);

    NPT_Map<NPT_String,A> a1_map;
    NPT_Map<NPT_String,A> a2_map;
    a1_map["hello"] = A(1,2);
    a1_map["bla"]   = A(2,3);
    a1_map["youyou"]= A(3,4);
    a2_map["bla"]   = A(2,3);
    a2_map["youyou"]= A(3,4);
    a2_map["hello"] = A(1,2);
    NPT_ASSERT(a1_map == a2_map);
    a1_map["foo"] = A(0,0);
    NPT_ASSERT(a1_map != a2_map);
    a2_map["foo"] = A(0,0);
    NPT_ASSERT(a1_map == a2_map);
    a2_map["foo"] = A(7,8);
    NPT_ASSERT(a1_map != a2_map);
    a2_map["foo"] = A(0,0);
    a1_map["bir"] = A(0,0);
    a2_map["bar"] = A(0,0);
    NPT_ASSERT(a1_map.GetEntryCount() == a2_map.GetEntryCount());
    NPT_ASSERT(a1_map != a2_map);
    NPT_ASSERT(!(a1_map == a2_map));

    return 0;
}
