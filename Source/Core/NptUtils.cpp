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
#include "NptConfig.h"
#include "NptDebug.h"
#include "NptUtils.h"

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
|    ATX_BytesFromInt16Be
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
|       NPT_ByteToHex
+---------------------------------------------------------------------*/
void NPT_ByteToHex(NPT_Byte b, char* buffer)
{
    buffer[0] = NPT_NibbleToHex((b>>4) & 0x0F);
    buffer[1] = NPT_NibbleToHex(b      & 0x0F);
}





