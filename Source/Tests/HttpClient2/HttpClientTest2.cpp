 /*****************************************************************
|
|      HTTP Client Test Program 2
|
|      (c) 2001-2009 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "NptDebug.h"

/*----------------------------------------------------------------------
|       ShowResponse
+---------------------------------------------------------------------*/
static void
ShowResponse(NPT_HttpResponse* response)
{
    bool check_available = true;//true;
    
    // show entity
    NPT_HttpEntity* entity = response->GetEntity();
    if (entity == NULL) return;
    
    NPT_Console::OutputF("ENTITY: length=%lld, type=%s, encoding=%s\n",
                         entity->GetContentLength(),
                         entity->GetContentType().GetChars(),
                         entity->GetContentEncoding().GetChars());

    NPT_DataBuffer buffer(65536);
    NPT_Result result;
    NPT_InputStreamReference input;
    entity->GetInputStream(input);
    
    NPT_TimeStamp start;
    NPT_System::GetCurrentTimeStamp(start);
    float total_read = 0.0f;
    for (;;) {
        NPT_Size bytes_read = 0;
        NPT_LargeSize available = 0;
        NPT_Size to_read = 65536;
        if (check_available) {
            input->GetAvailable(available);
            if ((NPT_Size)available < to_read) to_read = (NPT_Size)available;
            if (to_read == 0) {
                to_read = 1;
                NPT_TimeStamp sleep_time(0.01f);
                NPT_System::Sleep(sleep_time);
            }
        }
        result = input->Read(buffer.UseData(), to_read, &bytes_read);
        if (NPT_FAILED(result)) break;
        total_read += bytes_read;
        NPT_TimeStamp now;
        NPT_System::GetCurrentTimeStamp(now);
        NPT_TimeStamp duration = now-start;
        NPT_Console::OutputF("%6d avail, read %6d bytes, %6.3f KB/s\n", (int)available, bytes_read, (float)((total_read/1024.0)/(double)duration));
    }
}

/*----------------------------------------------------------------------
|       TestHttpGet
+---------------------------------------------------------------------*/
static void 
TestHttpGet(const char* arg)
{
    NPT_HttpUrl url(arg);
    NPT_HttpRequest request(url, NPT_HTTP_METHOD_GET);
    NPT_HttpClient client;
    NPT_HttpResponse* response;

    NPT_Result result = client.SendRequest(request, response);
    NPT_Debug("SendRequest returned %d\n", result);
    if (NPT_FAILED(result)) return;

    ShowResponse(response);

    delete response;
}

/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int
main(int argc, char** argv)
{
    // parse args
    if (argc != 2) {
        NPT_Debug("HttpClientTest2: missing URL argument\n");
        return 1;
    }

    TestHttpGet(argv[1]);

    return 0;
}
