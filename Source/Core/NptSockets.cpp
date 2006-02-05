/*****************************************************************
|
|      Neptune - Sockets
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "NptSockets.h"
#include "NptUtils.h"

/*----------------------------------------------------------------------
|       NPT_IpAddress::Any
+---------------------------------------------------------------------*/
NPT_IpAddress NPT_IpAddress::Any;

/*----------------------------------------------------------------------
|       NPT_IpAddress::NPT_IpAddress
+---------------------------------------------------------------------*/
NPT_IpAddress::NPT_IpAddress()
{
    m_Address[0] = m_Address[1] = m_Address[2] = m_Address[3] = 0;
}

/*----------------------------------------------------------------------
|       NPT_IpAddress::NPT_IpAddress
+---------------------------------------------------------------------*/
NPT_IpAddress::NPT_IpAddress(unsigned long address)
{
	Set(address);
}

/*----------------------------------------------------------------------
|       NPT_IpAddress::Parse
+---------------------------------------------------------------------*/
NPT_Result
NPT_IpAddress::Parse(const char* name)
{
    const char*   look = name;
    unsigned char step = 0;
    unsigned char c;
    unsigned int  accumulator = 0;
    
    while ((c = *look++) && step < 4) {
        if (c >= '0' && c <= '9') {
            /* a numerical character */
            accumulator = accumulator*10 + (c - '0');
            continue;
        }
        if (c == '.') {
            m_Address[step++] = accumulator;
            accumulator = 0;
        }
    }

    if (c == '\0' && step == 3) {
        m_Address[3] = accumulator;
        return NPT_SUCCESS;
    } else {
        return NPT_FAILURE;
    }
} 

/*----------------------------------------------------------------------
|       NPT_IpAddress::AsLong
+---------------------------------------------------------------------*/
unsigned long
NPT_IpAddress::AsLong() const
{
    return 
        (((unsigned long)m_Address[0])<<24) |
        (((unsigned long)m_Address[1])<<16) |
        (((unsigned long)m_Address[2])<< 8) |
        (((unsigned long)m_Address[3]));
}

/*----------------------------------------------------------------------
|       NPT_IpAddress::ToString
+---------------------------------------------------------------------*/
NPT_String
NPT_IpAddress::ToString() const
{
    NPT_String address;
    address.Reserve(16);
    address += NPT_String::FromInteger(m_Address[0]);
    address += '.';
    address += NPT_String::FromInteger(m_Address[1]);
    address += '.';
    address += NPT_String::FromInteger(m_Address[2]);
    address += '.';
    address += NPT_String::FromInteger(m_Address[3]);

    return address;
}

/*----------------------------------------------------------------------
|       NPT_IpAddress::Set
+---------------------------------------------------------------------*/
NPT_Result    
NPT_IpAddress::Set(const unsigned char bytes[4])
{
    m_Address[0] = bytes[0];
    m_Address[1] = bytes[1];
    m_Address[2] = bytes[2];
    m_Address[3] = bytes[3];

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_IpAddress::Set
+---------------------------------------------------------------------*/
NPT_Result    
NPT_IpAddress::Set(unsigned long address)
{
    m_Address[0] = (unsigned char)((address >> 24) & 0xFF);
    m_Address[1] = (unsigned char)((address >> 16) & 0xFF);
    m_Address[2] = (unsigned char)((address >>  8) & 0xFF);
    m_Address[3] = (unsigned char)((address      ) & 0xFF);

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_MacAddress::NPT_MacAddress
+---------------------------------------------------------------------*/
NPT_MacAddress::NPT_MacAddress(Type                  type,
                               const unsigned char*  address, 
                               unsigned int          length)
{
    SetAddress(type, address, length);
}

/*----------------------------------------------------------------------
|       NPT_MacAddress::SetAddress
+---------------------------------------------------------------------*/
void
NPT_MacAddress::SetAddress(Type                 type,
                           const unsigned char* address, 
                           unsigned int         length)
{
    m_Type = type;
    if (length > NPT_NETWORK_MAX_MAC_ADDRESS_LENGTH) {
        length = NPT_NETWORK_MAX_MAC_ADDRESS_LENGTH;
    }
    m_Length = length;
    for (unsigned int i=0; i<length; i++) {
        m_Address[i] = address[i];
    }
}

/*----------------------------------------------------------------------
|       NPT_MacAddress::ToString
+---------------------------------------------------------------------*/
NPT_String
NPT_MacAddress::ToString() const
{
    NPT_String result;
 
    if (m_Length) {
        char s[3*NPT_NETWORK_MAX_MAC_ADDRESS_LENGTH];
        const char hex[17] = "0123456789abcdef";
        for (unsigned int i=0; i<m_Length; i++) {
            s[i*3  ] = hex[m_Address[i]>>4];
            s[i*3+1] = hex[m_Address[i]&0xf];
            s[i*3+2] = ':';
        }
        s[3*m_Length-1] = '\0';
        result = s;
    }

    return result;
}

/*----------------------------------------------------------------------
|       NPT_NetworkInterface::NPT_NetworkInterface
+---------------------------------------------------------------------*/ 
NPT_NetworkInterface::NPT_NetworkInterface(const char*           name,
                                           const NPT_MacAddress& mac,
                                           NPT_Flags             flags) :
    m_Name(name),
    m_MacAddress(mac),
    m_Flags(flags)
{
}

/*----------------------------------------------------------------------
|       NPT_NetworkInterface::AddAddress
+---------------------------------------------------------------------*/ 
NPT_Result
NPT_NetworkInterface::AddAddress(const NPT_NetworkInterfaceAddress& address)
{
    return m_Addresses.Add(address);
}


