/*****************************************************************
|
|      Neptune - Utils
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <math.h>

#include "NptConfig.h"
#include "NptDebug.h"
#include "NptUtils.h"
#include "NptResults.h"

/*----------------------------------------------------------------------
|       NPT_BytesToInt32Be
+---------------------------------------------------------------------*/
unsigned long 
NPT_BytesToInt32Be(const unsigned char* bytes)
{
    return 
        ( ((unsigned long)bytes[0])<<24 ) |
        ( ((unsigned long)bytes[1])<<16 ) |
        ( ((unsigned long)bytes[2])<<8  ) |
        ( ((unsigned long)bytes[3])     );    
}

/*----------------------------------------------------------------------
|       NPT_BytesToInt16Be
+---------------------------------------------------------------------*/
unsigned short 
NPT_BytesToInt16Be(const unsigned char* bytes)
{
    return 
        ( ((unsigned short)bytes[0])<<8  ) |
        ( ((unsigned short)bytes[1])     );    
}

/*----------------------------------------------------------------------
|    NPT_BytesFromInt32Be
+---------------------------------------------------------------------*/
void 
NPT_BytesFromInt32Be(unsigned char* buffer, unsigned long value)
{
    buffer[0] = (unsigned char)(value>>24) & 0xFF;
    buffer[1] = (unsigned char)(value>>16) & 0xFF;
    buffer[2] = (unsigned char)(value>> 8) & 0xFF;
    buffer[3] = (unsigned char)(value    ) & 0xFF;
}

/*----------------------------------------------------------------------
|    NPT_BytesFromInt16Be
+---------------------------------------------------------------------*/
void 
NPT_BytesFromInt16Be(unsigned char* buffer, unsigned short value)
{
    buffer[0] = (unsigned char)((value>> 8) & 0xFF);
    buffer[1] = (unsigned char)((value    ) & 0xFF);
}

#if !defined(NPT_CONFIG_HAVE_SNPRINTF)
/*----------------------------------------------------------------------
|       NPT_FormatString
+---------------------------------------------------------------------*/
int 
NPT_FormatString(char* str, NPT_Size size, const char* format, ...)
{
    NPT_ASSERT(0); // not implemented yet
    return 0;
}
#endif // NPT_CONFIG_HAVE_SNPRINTF

/*----------------------------------------------------------------------
|       NPT_NibbleToHex
+---------------------------------------------------------------------*/
static char NPT_NibbleToHex(unsigned int nibble)
{
    NPT_ASSERT(nibble < 16);
    return (nibble < 10) ? ('0' + nibble) : ('A' + (nibble-10));
}

/*----------------------------------------------------------------------
|       NPT_HexToNibble
+---------------------------------------------------------------------*/
static unsigned int NPT_HexToNibble(char hex)
{
    if (hex >= 'a') {
        return ((hex - 'a') + 10);
    } else if (hex >= 'A') {
        return ((hex - 'A') + 10);
    } else {
        return (hex - '0');
    }
}

/*----------------------------------------------------------------------
|       NPT_ByteToHex
+---------------------------------------------------------------------*/
void NPT_ByteToHex(NPT_Byte b, char* buffer)
{
    buffer[0] = NPT_NibbleToHex((b>>4) & 0x0F);
    buffer[1] = NPT_NibbleToHex(b      & 0x0F);
}

/*----------------------------------------------------------------------
|       NPT_HexToByte
+---------------------------------------------------------------------*/
void NPT_HexToByte(const char* buffer, NPT_Byte& b)
{
    NPT_ASSERT(NPT_StringLength(buffer) >= 2);
    b = (NPT_HexToNibble(buffer[0]) << 4) | NPT_HexToNibble(buffer[1]);
}

/*----------------------------------------------------------------------
|    NPT_ParseInteger
+---------------------------------------------------------------------*/
NPT_Result 
NPT_ParseInteger(const char* str, long& result, bool relaxed)
{
    // safe default value
    result = 0;

    if (str == NULL) {
        return NPT_ERROR_INVALID_PARAMETERS;
    }

    // ignore leading whitespace
    if (relaxed) {
        while (*str == ' ' || *str == '\t') {
            str++;
        }
    }
    if (*str == '\0') {
        return NPT_ERROR_INVALID_PARAMETERS;
    }

    // check for sign
    bool negative = false;
    if (*str == '-') {
        // negative number
        negative = true; 
        str++;
    } else if (*str == '+') {
        // skip the + sign
        str++;
    }

    // parse the digits
    bool empty    = true;
    long value    = 0;
    char c;
    while ((c = *str++)) {
        if (c >= '0' && c <= '9') {
            value = 10*value + (c-'0');
            empty = false;
        } else {
            if (relaxed) {
                break;
            } else {
                return NPT_ERROR_INVALID_PARAMETERS;
            }
        } 
    }

    // check that the value was non empty
    if (empty) {
        return NPT_ERROR_INVALID_PARAMETERS;
    }

    // return the result
    result = negative ? -value : value;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|    NPT_ParseFloat
+---------------------------------------------------------------------*/
NPT_Result 
NPT_ParseFloat(const char* str, float& result, bool relaxed)
{
    // safe default value 
    result = 0.0f;

    // check params
    if (str == NULL || *str == '\0') {
        return NPT_ERROR_INVALID_PARAMETERS;
    }

    // ignore leading whitespace
    if (relaxed) {
        while (*str == ' ' || *str == '\t') {
            str++;
        }
    }
    if (*str == '\0') {
        return NPT_ERROR_INVALID_PARAMETERS;
    }

    // check for sign
    bool  negative = false;
    if (*str == '-') {
        // negative number
        negative = true; 
        str++;
    } else if (*str == '+') {
        // skip the + sign
        str++;
    }

    // parse the digits
    bool  after_radix = false;
    bool  empty = true;
    float value = 0.0f;
    float decimal = 10.0f;
    char  c;
    while ((c = *str++)) {
        if (c == '.') {
            if (after_radix || (*str < '0' || *str > '9')) {
                return NPT_ERROR_INVALID_PARAMETERS;
            } else {
                after_radix = true;
            }
        } else if (c >= '0' && c <= '9') {
            empty = false;
            if (after_radix) {
                value += (float)(c-'0')/decimal;
                decimal *= 10.0f;
            } else {
                value = 10.0f*value + (float)(c-'0');
            }
        } else if (c == 'e' || c == 'E') {
            // exponent
            if (*str == '+' || *str == '-' || (*str >= '0' && *str <= '9')) {
                long exponent = 0;
                if (NPT_SUCCEEDED(NPT_ParseInteger(str, exponent, relaxed))) {
                    value *= (float)pow(10.0f, (float)exponent);
                    break;
                } else {
                    return NPT_ERROR_INVALID_PARAMETERS;
                }
            } else {
                return NPT_ERROR_INVALID_PARAMETERS;
            }
        } else {
            if (relaxed) {
                break;
            } else {
                return NPT_ERROR_INVALID_PARAMETERS;
            }
        } 
    }

    // check that the value was non empty
    if (empty) {
        return NPT_ERROR_INVALID_PARAMETERS;
    }

    // return the result
    result = negative ? -value : value;
    return NPT_SUCCESS;
}


