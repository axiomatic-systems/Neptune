/*****************************************************************
|
|      Misc Test Program 1
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
|       main
+---------------------------------------------------------------------*/
int
main(int /*argc*/, char** /*argv*/)
{
    NPT_Result result;

    NPT_String t = "hello";
    NPT_String base64;
    result = NPT_Base64::Encode((const NPT_Byte*)t.GetChars(), t.GetLength(), base64);
    NPT_ASSERT(NPT_SUCCEEDED(result));
    NPT_ASSERT(base64 == "aGVsbG8=");

    t = "hello!";
    result = NPT_Base64::Encode((const NPT_Byte*)t.GetChars(), t.GetLength(), base64);
    NPT_ASSERT(NPT_SUCCEEDED(result));
    NPT_ASSERT(base64 == "aGVsbG8h");

    t = "hello!!";
    result = NPT_Base64::Encode((const NPT_Byte*)t.GetChars(), t.GetLength(), base64);
    NPT_ASSERT(NPT_SUCCEEDED(result));
    NPT_ASSERT(base64 == "aGVsbG8hIQ==");
    
    unsigned char random_bytes[] = {
        0xc7, 0xee, 0x49, 0x9e, 0x2c, 0x8b, 0x1c, 0x16, 0x9e, 0x7f, 0x30, 0xd0,
        0xc6, 0x12, 0x30, 0x80, 0x81, 0xcd, 0x20, 0x20, 0x26, 0xaf, 0x4f, 0xd6,
        0xfc, 0x86, 0x2e, 0x85, 0xf3, 0x10, 0x38, 0x2b, 0x0e, 0xbb, 0x80, 0x68,
        0xbe, 0xff, 0x1c, 0xdc, 0x72, 0xb5, 0x0d, 0x8f, 0x8e, 0x6c, 0x09, 0x63,
        0xba, 0x21, 0x23, 0xb2, 0x24, 0x17, 0xd3, 0x17, 0x69, 0x44, 0x77, 0x11,
        0x36, 0x6a, 0x6e, 0xf2, 0x44, 0x87, 0xa1, 0xd3, 0xf3, 0x1f, 0x6c, 0x38,
        0x22, 0x4a, 0x44, 0x70, 0x66, 0xef, 0x8c, 0x3a, 0x51, 0xc8, 0xee, 0x85,
        0x00, 0x25, 0x93, 0x10, 0x2e, 0x0b, 0x1b, 0x03, 0x94, 0x47, 0x05, 0x22,
        0xd0, 0xc4, 0xec, 0x2e, 0xcc, 0xbc, 0xbb, 0x67, 0xfd, 0xec, 0x0e, 0xb1,
        0x3f, 0xbc, 0x82, 0xe0, 0xa7, 0x9c, 0xf3, 0xae, 0xbd, 0xb7, 0xab, 0x02,
        0xf1, 0xd9, 0x17, 0x4c, 0x9d, 0xeb, 0xe2, 0x00, 0x1e, 0x19, 0x6e, 0xb3,
        0xfd, 0x7d, 0xea, 0x49, 0x85, 0x43, 0x2f, 0x56, 0x81, 0x89, 0xba, 0x71,
        0x37, 0x10, 0xb5, 0x74, 0xab, 0x90, 0x4d, 0xc4, 0xd1, 0x0d, 0x8d, 0x6f,
        0x01, 0xf5, 0x2c, 0xc9, 0x1a, 0x79, 0xa1, 0x41, 0x71, 0x2b, 0xfb, 0xf3,
        0xd5, 0xe4, 0x2a, 0xf5, 0xad, 0x80, 0x7a, 0x03, 0xff, 0x5f, 0x45, 0x8c,
        0xec, 0x6a, 0x4b, 0x05, 0xe3, 0x65, 0x19, 0x70, 0x05, 0xad, 0xc4, 0xb8,
        0x4e, 0x9e, 0x9a, 0x36, 0x4a, 0x86, 0x9d, 0xf5, 0x99, 0xcb, 0x00, 0xb8,
        0xb9, 0xa7, 0x86, 0x18, 0xfc, 0x9a, 0xe7, 0x00, 0x6a, 0x67, 0xfa, 0x42,
        0x9d, 0xff, 0x4d, 0x7a, 0xe4, 0xe8, 0x03, 0x88, 0xff, 0x60, 0xe1, 0x8d,
        0x09, 0x5f, 0x6f, 0xde, 0x6b
    };
    NPT_Array<unsigned char> random(random_bytes, NPT_ARRAY_SIZE(random_bytes));

    NPT_DataBuffer data;
    t = "x+5JniyLHBaefzDQxhIwgIHNICAmr0/W/IYuhfMQOCsOu4Bovv8c3HK1DY+ObAlj\r\n"
        "uiEjsiQX0xdpRHcRNmpu8kSHodPzH2w4IkpEcGbvjDpRyO6FACWTEC4LGwOURwUi\r\n"
        "0MTsLsy8u2f97A6xP7yC4Kec8669t6sC8dkXTJ3r4gAeGW6z/X3qSYVDL1aBibpx\r\n"
        "NxC1dKuQTcTRDY1vAfUsyRp5oUFxK/vz1eQq9a2AegP/X0WM7GpLBeNlGXAFrcS4\r\n"
        "Tp6aNkqGnfWZywC4uaeGGPya5wBqZ/pCnf9NeuToA4j/YOGNCV9v3ms=";
    result = NPT_Base64::Decode(t.GetChars(), t.GetLength(), data);
    NPT_ASSERT(NPT_SUCCEEDED(result));
    NPT_ASSERT(data.GetDataSize() == 233);
    NPT_Array<unsigned char> verif(data.GetData(), data.GetDataSize());
    NPT_ASSERT(verif == random);

    result = NPT_Base64::Encode(&random[0], random.GetItemCount(), base64, NPT_BASE64_PEM_BLOCKS_PER_LINE);
    NPT_ASSERT(NPT_SUCCEEDED(result));
    NPT_ASSERT(base64 == t);

    NPT_String t_url = t;
    t.Replace('/', '_');
    t.Replace('+', '-');
    result = NPT_Base64::Encode(&random[0], random.GetItemCount(), base64, NPT_BASE64_PEM_BLOCKS_PER_LINE, true);
    NPT_ASSERT(NPT_SUCCEEDED(result));
    NPT_ASSERT(base64 == t);
    
    t = "76768484767685839";
    result = NPT_Base64::Decode(t.GetChars(), t.GetLength(), data);
    NPT_ASSERT(result == NPT_ERROR_INVALID_FORMAT);

    t = "76869=978686";
    result = NPT_Base64::Decode(t.GetChars(), t.GetLength(), data);
    NPT_ASSERT(result == NPT_ERROR_INVALID_FORMAT);

    t = "7686=8978686";
    result = NPT_Base64::Decode(t.GetChars(), t.GetLength(), data);
    NPT_ASSERT(result == NPT_ERROR_INVALID_FORMAT);

    t = "7686==978686";
    result = NPT_Base64::Decode(t.GetChars(), t.GetLength(), data);
    NPT_ASSERT(result == NPT_ERROR_INVALID_FORMAT);

    return 0;
}
