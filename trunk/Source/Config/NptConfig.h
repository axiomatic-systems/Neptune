/*****************************************************************
|
|   File: NptConfig.h
|
|   Neptune - Configuration
|
|   (c) 2002-2003 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

#ifndef _NPT_CONFIG_H_
#define _NPT_CONFIG_H_

/*----------------------------------------------------------------------
|   defaults
+---------------------------------------------------------------------*/
#define NPT_CONFIG_HAVE_ASSERT_H
#define NPT_CONFIG_HAVE_STD_C
#define NPT_CONFIG_HAVE_STDLIB_H
#define NPT_CONFIG_HAVE_STDIO_H
#define NPT_CONFIG_HAVE_STRING_H

/*----------------------------------------------------------------------
|   standard C runtime
+---------------------------------------------------------------------*/
#if defined(NPT_CONFIG_HAVE_STD_C)
#define NPT_CONFIG_HAVE_MALLOC
#define NPT_CONFIG_HAVE_CALLOC
#define NPT_CONFIG_HAVE_REALLOC
#define NPT_CONFIG_HAVE_FREE
#define NPT_CONFIG_HAVE_MEMCPY
#define NPT_CONFIG_HAVE_MEMSET
#define NPT_CONFIG_HAVE_MEMCMP
#endif /* NPT_CONFIG_HAS_STD_C */

#if defined(NPT_CONFIG_HAVE_STRING_H)
#define NPT_CONFIG_HAVE_STRCMP
#define NPT_CONFIG_HAVE_STRNCMP
#define NPT_CONFIG_HAVE_STRDUP
#define NPT_CONFIG_HAVE_STRLEN
#define NPT_CONFIG_HAVE_STRCPY
#define NPT_CONFIG_HAVE_STRNCPY
#endif /* NPT_CONFIG_HAVE_STRING_H */

#if defined(NPT_CONFIG_HAVE_STDIO_H)
#define NPT_CONFIG_HAVE_SPRINTF
#define NPT_CONFIG_HAVE_SNPRINTF
#define NPT_CONFIG_HAVE_VSPRINTF
#define NPT_CONFIG_HAVE_VSNPRINTF
#endif /* NPT_CONFIG_HAVE_STDIO_H */

/*----------------------------------------------------------------------
|   standard C++ runtime
+---------------------------------------------------------------------*/
#define NPT_CONFIG_HAVE_NEW_H

/*----------------------------------------------------------------------
|   sockets
+---------------------------------------------------------------------*/
#define NPT_CONFIG_HAVE_SOCKADDR_SA_LEN

/*----------------------------------------------------------------------
|   platform specifics
+---------------------------------------------------------------------*/
/* Windows 32 */
#if defined(WIN32)
#if !defined(STRICT)
#define STRICT
#endif
#define vsnprintf _vsnprintf
#define snprintf _snprintf
#endif

/* QNX */
#if defined(__QNX__)
#endif

/* cygwin */
#if defined(__CYGWIN__)
#undef NPT_CONFIG_HAVE_SOCKADDR_SA_LEN
#endif

/* linux */
#if defined(__linux__)
#undef NPT_CONFIG_HAVE_SOCKADDR_SA_LEN
#endif

/*----------------------------------------------------------------------
|   compiler specifics
+---------------------------------------------------------------------*/
/* GCC */
#if defined(__GNUC__)
#define NPT_COMPILER_UNUSED(p) (void)p
#else
#define NPT_COMPILER_UNUSED(p) 
#endif

/* TriMedia C/C++ Compiler */
#if defined(__TCS__)
#undef NPT_CONFIG_HAVE_ASSERT_H
#undef NPT_CONFIG_HAVE_SNPRINTF
#undef NPT_CONFIG_HAVE_VSNPRINTF
#endif

/* palmos compiler */
#if defined(__PALMOS__)
#if __PALMOS__ <= 0x05000000
#undef NPT_CONFIG_HAVE_ASSERT_H
#undef NPT_CONFIG_HAVE_SNPRINTF
#undef NPT_CONFIG_HAVE_VSNPRINTF
#endif
#endif

/* Microsoft C/C++ Compiler */
#if defined(_MSC_VER)
#if _MSC_VER >= 1400
typedef __w64 long NPT_PointerLong;
#define NPT_POINTER_TO_LONG(_p) ((long)(NPT_PointerLong) (_p) )
#define NPT_CONFIG_HAVE_FOPEN_S
#endif
#endif

/* Symbian */
#if defined(__SYMBIAN32__)
#undef NPT_CONFIG_HAVE_NEW_H
#include "e32std.h"
#define explicit
#endif

/*----------------------------------------------------------------------
|   defaults
+---------------------------------------------------------------------*/
#ifndef NPT_POINTER_TO_LONG
#define NPT_POINTER_TO_LONG(_p) ((long)(_p))
#endif

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#if defined(DMALLOC)
#include <dmalloc.h>
#endif

#endif // _NPT_CONFIG_H_ 
