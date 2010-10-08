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

#define LOG_FORMAT "%30s,%3d,%8d,[%30s],%s\n"

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
    if (NPT_FAILED(result)) {
        printf(LOG_FORMAT, NPT_ResultText(result), 0, 0, "", arg);
        return;
    } 
    const NPT_String* server = response->GetHeaders().GetHeaderValue("Server");
    printf(LOG_FORMAT, "NPT_SUCCESS", response->GetStatusCode(), (int)response->GetEntity()->GetContentLength(), server?server->GetChars():"", arg);

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
        NPT_Debug("HttpClientTest2: missing URL list argument\n");
        return 1;
    }

    NPT_DataBuffer list_buffer;
    NPT_File::Load(argv[1], list_buffer);
    NPT_String list_string;
    list_string.Assign((const char*)list_buffer.GetData(), list_buffer.GetDataSize());
    NPT_List<NPT_String> urls = list_string.Split("\n");
    printf("urls: %d\n", urls.GetItemCount());
    for (NPT_List<NPT_String>::Iterator it = urls.GetFirstItem(); it; ++it) {
        TestHttpGet((*it).GetChars());
    }

    return 0;
}
