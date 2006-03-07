/*****************************************************************
|
|      Neptune Utils
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

#ifndef _NPT_UTILS_H_
#define _NPT_UTILS_H_

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "NptConfig.h"
#include "NptTypes.h"

/*----------------------------------------------------------------------
|       macros
+---------------------------------------------------------------------*/
#define NPT_ARRAY_SIZE(_a) (sizeof(_a)/sizeof((_a)[0]))

/*----------------------------------------------------------------------
|       byte I/O
+---------------------------------------------------------------------*/
extern void NPT_BytesFromInt32Be(unsigned char* buffer, unsigned long value);
extern void NPT_BytesFromInt16Be(unsigned char* buffer, unsigned short value);
extern unsigned long NPT_BytesToInt32Be(const unsigned char* buffer);
extern unsigned short NPT_BytesToInt16Be(const unsigned char* buffer);

extern void NPT_BytesFromInt32Le(unsigned char* buffer, unsigned long value);
extern void NPT_BytesFromInt16Le(unsigned char* buffer, unsigned short value);
extern unsigned long NPT_BytesToInt32Le(const unsigned char* buffer);
extern unsigned short NPT_BytesToInt16Le(const unsigned char* buffer);

extern void NPT_ByteToHex(NPT_Byte b, char* buffer);
extern void NPT_HexToByte(const char* buffer, NPT_Byte& b);

/*----------------------------------------------------------------------
|    conversion utilities
+---------------------------------------------------------------------*/
extern NPT_Result 
NPT_ParseFloat(const char* str, float& result, bool relaxed = true);

extern NPT_Result 
NPT_ParseInteger(const char* str, long& result, bool relaxed = true);

/*----------------------------------------------------------------------
|       string utils
+---------------------------------------------------------------------*/
#if defined (NPT_CONFIG_HAVE_STDIO_H)
#include <stdio.h>
#endif

#if defined (NPT_CONFIG_HAVE_STRING_H)
#include <string.h>
#endif

#if defined (NPT_CONFIG_HAVE_SNPRINTF)
#define NPT_FormatString snprintf
#else
int NPT_FormatString(char* str, NPT_Size size, const char* format, ...);
#endif

#if defined(NPT_CONFIG_HAVE_MEMCPY)
#define NPT_CopyMemory memcpy
#else
extern void NPT_CopyMemory(void* dest, void* src, NPT_Size size);
#endif

#if defined(NPT_CONFIG_HAVE_STRCMP)
#define NPT_StringsEqual(s1, s2) \
(strcmp((s1), (s2)) == 0)
#else
extern int NPT_StringsEqual(const char* s1, const char* s2);
#endif

#if defined(NPT_CONFIG_HAVE_STRNCMP)
#define NPT_StringsEqualN(s1, s2, n) \
(strncmp((s1), (s2), (n)) == 0)
#else
extern int NPT_StringsEqualN(const char* s1, const char* s2, unsigned long size);
#endif

#if defined(NPT_CONFIG_HAVE_STRLEN)
#define NPT_StringLength(s) \
(NPT_Size)(strlen(s))
#else
extern unsigned long NPT_StringLength(const char* s);
#endif

#if defined(NPT_CONFIG_HAVE_MEMSET)
#define NPT_SetMemory memset
#else
extern void NPT_SetMemory(void* dest, int c, NPT_Size size);
#endif

#endif // _NPT_UTILS_H_
