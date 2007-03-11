/*****************************************************************
|
|   Neptune Utils, Symbian
|
|   (c) 2001-2006 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "stddef.h"
#include "stdarg.h"
#include "string.h"

#include "NptResults.h"
#include "NptDebug.h"
#include "NptStrings.h"

/*----------------------------------------------------------------------
|       _NPT_FormatStringWriteInt
+---------------------------------------------------------------------*/
static int
_NPT_FormatStringWriteInt(char *buf, int len, int val)
{
    NPT_ASSERT(buf != NULL);

    NPT_String tmp = NPT_String::FromInteger(val);
    int tmplen = tmp.GetLength();
    if (tmplen < len) {
        len = tmplen;
    }
    _e32memcpy(buf, tmp.GetChars(), len);
    
    return tmplen;
}

/*----------------------------------------------------------------------
|       _NPT_FormatStringWriteChar
+---------------------------------------------------------------------*/
static int
_NPT_FormatStringWriteChar(char *buf, int len, char val)
{
    NPT_ASSERT(buf != NULL);

    if (len > 0) {    
        *buf = val;
    }
    
    return 1;
}

#if 0
/*----------------------------------------------------------------------
|       _NPT_FormatStringWriteFloat
+---------------------------------------------------------------------*/
static int
_NPT_FormatStringWriteFloat(char *buf, int len, double val)
{
    char tmp[32];
    int tmplen;
    
    NPT_ASSERT(buf != NULL);

    if (NPT_FAILED(NPT_FloatToString(val, tmp, 32))) {
        return 0;
    }
    
    tmplen = strlen(tmp);
    if (tmplen < len) {
        len = tmplen;
    }
    _e32memcpy(buf, tmp, len);
    
    return tmplen;
}
#endif

/*----------------------------------------------------------------------
|       _NPT_FormatStringWriteString
+---------------------------------------------------------------------*/
static int
_NPT_FormatStringWriteString(char *buf, int len, const char* val)
{
    int vallen;
    
    NPT_ASSERT(buf != NULL);
    NPT_ASSERT(val != NULL);
    
    vallen = strlen(val);
    if (vallen < len) {
        len = vallen;
    }
    
    _e32memcpy(buf, val, len);
    
    return vallen;
}

/*----------------------------------------------------------------------
|       NPT_FormatStringVN
+---------------------------------------------------------------------*/
int
NPT_FormatStringVN(char *buffer, size_t count, const char *format, va_list argptr)
{
    int c = 0; /* Contains the length (or the necessary length when 'count' is less than 'c') of the formatted string not counting the null terminator. */
    char *cur = buffer;
    int fi, sub;
    
    /* count is the size of the buffer not including the null terminator. */
    --count;
    
    for (fi = 0; format[fi] != '\0'; ++fi) {
        if (format[fi] == '%') {
            for (sub = 1; format[fi+sub] != '\0'; ++sub) {
                char finished = 1;
                switch (format[fi+sub]) {
                    case 'd':
                    case 'i':
                        c += _NPT_FormatStringWriteInt(cur, count - c, va_arg(argptr, int));
                        break;
#if 0                        
                    case 'f':
                    case 'F':
                        c += _NPT_FormatStringWriteFloat(cur, count - c, va_arg(argptr, double));
                        break;
#endif                        
                    case 'c':
                        c += _NPT_FormatStringWriteChar(cur, count - c, va_arg(argptr, int)); /* pass an int because of an auto promotion error message */
                        break;
                    
                    case 's':
                        c += _NPT_FormatStringWriteString(cur, count - c, va_arg(argptr, char *));
                        break;
                        
                    case '%':
                        c += _NPT_FormatStringWriteChar(cur, count - c, '%');
                        break;
                    
                    default:
                        /* just write unsupported/invalid format strings to the output */
                        c += _NPT_FormatStringWriteChar(cur, count - c, '%');
                        c += _NPT_FormatStringWriteChar(cur+1, count - c, format[fi+sub]);
                        break;
                }
                
                if (finished) {
                    fi += sub;
                    break;
                }
            }
        }
        else {
            /* just copy this over */
            if (c < count) {
                buffer[c++] = format[fi];
            }
        }
        
        cur = buffer + c;
    }
    
    /* null terminate the string */
    if (c < count) {
        buffer[c] = '\0';
    }
    else {
        buffer[count] = '\0';
    }
    
    return c;
}
