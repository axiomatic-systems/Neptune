/*****************************************************************
|
|      HTTP Client Test Program 2
|
|      (c) 2001-2011 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "NptDebug.h"

#define LOG_FORMAT "%30s,%3d,%8d, %8d, %8d, [%30s], %s\n"

/*----------------------------------------------------------------------
|       TestHttpGet
+---------------------------------------------------------------------*/
static void 
TestHttpGet(const char* arg, bool use_http_1_1)
{
    NPT_HttpUrl url(arg);
    NPT_HttpRequest request(url, NPT_HTTP_METHOD_GET);
    NPT_HttpClient client;
    NPT_HttpResponse* response;

    if (!url.IsValid()) return;
    if (use_http_1_1) request.SetProtocol(NPT_HTTP_PROTOCOL_1_1);

    NPT_TimeStamp before;
    NPT_System::GetCurrentTimeStamp(before);
    NPT_Result result = client.SendRequest(request, response);
    NPT_TimeStamp after;
    NPT_System::GetCurrentTimeStamp(after);
    NPT_UInt64 elapsed = (after-before).ToMillis();
    if (NPT_FAILED(result)) {
        printf(LOG_FORMAT, NPT_ResultText(result), 0, 0, 0, (int)elapsed, "", arg);
        return;
    } 
    NPT_DataBuffer payload;
    result = response->GetEntity()->Load(payload);
    int loaded = -1;
    if (NPT_SUCCEEDED(result))  {
        loaded = (int)payload.GetDataSize();
    }
    const NPT_String* server = response->GetHeaders().GetHeaderValue("Server");
    printf(LOG_FORMAT, "NPT_SUCCESS", response->GetStatusCode(), loaded, (int)response->GetEntity()->GetContentLength(), (int)elapsed, server?server->GetChars():"", arg);

    delete response;
}

/*----------------------------------------------------------------------
|       TestHttpPost
+---------------------------------------------------------------------*/
static void 
TestHttpPost(const char* arg, bool use_http_1_1)
{
    NPT_HttpUrl url(arg);
    NPT_HttpRequest request(url, NPT_HTTP_METHOD_POST);
    NPT_HttpClient client;
    NPT_HttpResponse* response;

    if (!url.IsValid()) return;
    if (use_http_1_1) request.SetProtocol(NPT_HTTP_PROTOCOL_1_1);

    NPT_HttpEntity* entity = new NPT_HttpEntity();
    entity->SetInputStream("blabla");
    request.SetEntity(entity);
    request.GetHeaders().SetHeader("Expect", "100-continue");
    
    NPT_TimeStamp before;
    NPT_System::GetCurrentTimeStamp(before);
    NPT_Result result = client.SendRequest(request, response);
    NPT_TimeStamp after;
    NPT_System::GetCurrentTimeStamp(after);
    NPT_UInt64 elapsed = (after-before).ToMillis();
    if (NPT_FAILED(result)) {
        printf(LOG_FORMAT, NPT_ResultText(result), 0, 0, 0, (int)elapsed, "", arg);
        return;
    } 
    NPT_DataBuffer payload;
    result = response->GetEntity()->Load(payload);
    int loaded = -1;
    if (NPT_SUCCEEDED(result))  {
        loaded = (int)payload.GetDataSize();
    }
    const NPT_String* server = response->GetHeaders().GetHeaderValue("Server");
    printf(LOG_FORMAT, "NPT_SUCCESS", response->GetStatusCode(), loaded, (int)response->GetEntity()->GetContentLength(), (int)elapsed, server?server->GetChars():"", arg);

    delete response;
}

/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int
main(int argc, char** argv)
{
    // parse args
    --argc; ++argv;
    bool         use_http_1_1 = false;
    unsigned int loops        = 1;
    bool         random       = false;
    bool         post         = false;
    unsigned int sleep        = 0;
    while (*argv) {
        if (NPT_StringsEqual(*argv, "--http-1-1")) {
            use_http_1_1 = true;
        } else if (NPT_StringsEqual(*argv, "--loops")) {
            NPT_ParseInteger(*++argv, loops);
        } else if (NPT_StringsEqual(*argv, "--post")) {
            post = true;
        } else if (NPT_StringsEqual(*argv, "--random")) {
            random = true;
        } else if (NPT_StringsEqual(*argv, "--sleep")) {
            NPT_ParseInteger(*++argv, sleep);
        } else {
            break;
        }
        ++argv;
    }
    if (*argv == NULL) {
        fprintf(stderr, "ERROR: missing URL list filename\n");
        return 1;
    }
    
    NPT_DataBuffer list_buffer;
    NPT_File::Load(*argv, list_buffer);
    NPT_String list_string;
    list_string.Assign((const char*)list_buffer.GetData(), list_buffer.GetDataSize());
    NPT_List<NPT_String> urls = list_string.Split("\n");
    printf("urls: %d\n", urls.GetItemCount());
    printf("loops: %d, random: %s, sleep: %d ms\n", loops, random?"true":"false", sleep);
    for (unsigned int i=0; i<loops; i++) {
        NPT_TimeStamp before;
        NPT_System::GetCurrentTimeStamp(before);
        for (unsigned int j=0; j<urls.GetItemCount(); j++) {
            unsigned int choice = j;
            if (random) {
                choice = NPT_System::GetRandomInteger()%urls.GetItemCount();
            }
            if (post) {
                TestHttpPost((*urls.GetItem(choice)).GetChars(), use_http_1_1);
            } else {
                TestHttpGet((*urls.GetItem(choice)).GetChars(), use_http_1_1);
            }
            if (sleep) {
                NPT_System::Sleep(NPT_TimeStamp(((float)sleep)/1000.0f));
            }
        }
        NPT_TimeStamp after;
        NPT_System::GetCurrentTimeStamp(after);
        float elapsed = (float)(after-before);
        printf("[%04d] TOTAL time elapsed = %f\n", i, elapsed);
    }
    
    return 0;
}
