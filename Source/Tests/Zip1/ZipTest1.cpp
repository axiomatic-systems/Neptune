/*****************************************************************
|
|      Zip Test Program 1
|
|      (c) 2001-2006 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "NptDebug.h"

#if defined(WIN32) && defined(_DEBUG)
#include <crtdbg.h>
#endif

#define CHECK(x)                                        \
    do {                                                \
      if (!(x)) {                                       \
        fprintf(stderr, "ERROR line %d \n", __LINE__);  \
        return -1;                                      \
      }                                                 \
    } while(0)

/*----------------------------------------------------------------------
|       test vectors
+---------------------------------------------------------------------*/
static unsigned char t1_gz[] = {
  0x1f, 0x8b, 0x08, 0x08, 0xf3, 0x53, 0xcf, 0x46, 0x02, 0x03, 0x68, 0x65,
  0x6c, 0x6c, 0x6f, 0x2e, 0x74, 0x78, 0x74, 0x00, 0xf3, 0x48, 0xcd, 0xc9,
  0xc9, 0x57, 0xf0, 0x4b, 0x2d, 0x28, 0x29, 0xcd, 0x4b, 0x05, 0x00, 0xf2,
  0xed, 0xac, 0x76, 0x0d, 0x00, 0x00, 0x00
};
static unsigned int t1_gz_len = 43;

static unsigned char t1[] = {
  0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x4e, 0x65, 0x70, 0x74, 0x75, 0x6e,
  0x65
};
static unsigned int t1_len = 13;

/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int
main(int /*argc*/, char** /*argv*/)
{
    // setup debugging
#if defined(WIN32) && defined(_DEBUG)
    int flags = _crtDbgFlag       | 
        _CRTDBG_ALLOC_MEM_DF      |
        _CRTDBG_DELAY_FREE_MEM_DF |
        _CRTDBG_CHECK_ALWAYS_DF;

    _CrtSetDbgFlag(flags);
    //AllocConsole();
    //freopen("CONOUT$", "w", stdout);
#endif 

    NPT_DataBuffer in1(t1_gz, t1_gz_len);
    NPT_DataBuffer out1;
    NPT_Result result = NPT_Zip::Inflate(in1, out1);
    CHECK(result == NPT_SUCCESS);
    CHECK(out1.GetDataSize() == t1_len);
    CHECK(NPT_MemoryEqual(out1.GetData(), t1, t1_len));
    
    NPT_DataBuffer in2(t1, t1_len);
    NPT_DataBuffer out2;
    NPT_DataBuffer out2_check;
    result = NPT_Zip::Deflate(in2, out2, NPT_ZIP_COMPRESSION_LEVEL_MAX, NPT_Zip::GZIP);
    CHECK(result == NPT_SUCCESS);
    result = NPT_Zip::Inflate(out2, out2_check);
    CHECK(result == NPT_SUCCESS);
    CHECK(out2_check.GetDataSize() == in2.GetDataSize());
    CHECK(NPT_MemoryEqual(t1, out2_check.GetData(), in2.GetDataSize()));
    
    // try with random data
    NPT_DataBuffer in3(300000);
    unsigned char* in3_p = in3.UseData();
    for (int i=0; i<300000; i++) {
        *in3_p++ = NPT_System::GetRandomInteger();
    }
    in3.SetDataSize(300000);
    NPT_DataBuffer out3;
    result = NPT_Zip::Deflate(in3, out3);
    CHECK(result == NPT_SUCCESS);
    NPT_DataBuffer out3_check;
    result = NPT_Zip::Inflate(out3, out3_check);
    CHECK(result == NPT_SUCCESS);
    CHECK(in3 == out3_check);

    // try with redundant data
    in3_p = in3.UseData();
    for (int i=0; i<200000; i+=4) {
        *in3_p++ = NPT_System::GetRandomInteger();
        *in3_p++ = 0;
        *in3_p++ = 0;
        *in3_p++ = 0;
    }
    result = NPT_Zip::Deflate(in3, out3);
    CHECK(result == NPT_SUCCESS);
    result = NPT_Zip::Inflate(out3, out3_check);
    CHECK(result == NPT_SUCCESS);
    CHECK(in3 == out3_check);

    return 0;
}
