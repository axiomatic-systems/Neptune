/*****************************************************************
|
|      Neptune - Common Definitions
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

#ifndef _NPT_COMMON_H_
#define _NPT_COMMON_H_

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "NptTypes.h"

/*----------------------------------------------------------------------
|       NPT_Exception
+---------------------------------------------------------------------*/
class NPT_Exception
{
public:
    // methods
    NPT_Exception(NPT_Result error) : m_Error(error) {}

    // members
    NPT_Result m_Error;
};

/*----------------------------------------------------------------------
|       NPT_PropertyValue
+---------------------------------------------------------------------*/
class NPT_PropertyValue
{
 public:
    // typedefs
    typedef enum {UNKNOWN, INTEGER, STRING} Type;

    // methods
    NPT_PropertyValue() : m_Type(UNKNOWN), m_Integer(0) {}
    NPT_PropertyValue(NPT_Integer value) : m_Type(INTEGER), m_Integer(value) {}
    NPT_PropertyValue(const char* value) : m_Type(STRING),  m_String(value)  {}

    // members
    Type m_Type;
    union {
        NPT_Integer m_Integer;
        const char* m_String;
    };
};

#endif // _NPT_COMMON_H_













