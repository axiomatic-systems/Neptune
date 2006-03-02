/*****************************************************************
|
|      HTTP Client Test Program 1
|
|      (c) 2001-2003 Gilles Boccon-Gibod
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

/*----------------------------------------------------------------------
|       TestUrlParser
+---------------------------------------------------------------------*/
static void
TestUrlParser(const char* url)
{
    NPT_HttpUrl url_object(url);
    NPT_Debug("Parsing URL: '%s'\n", url ? url : "null");
    if (url_object.IsValid()) {
        NPT_Debug("  --> host=%s, port=%d, path=%s\n", 
            (const char*)url_object.GetHost(),
            url_object.GetPort(),
            (const char*)url_object.GetPath());
    } else {
        NPT_Debug("  --> Invalid URL\n");
    }
}

/*----------------------------------------------------------------------
|       ShowResponse
+---------------------------------------------------------------------*/
static void
ShowResponse(NPT_HttpResponse* response)
{
    // show response info
    NPT_Debug("RESPONSE: protocol=%s, code=%d, reason=%s\n",
              response->GetProtocol().GetChars(),
              response->GetStatusCode(),
              response->GetReasonPhrase().GetChars());

    // show headers
    NPT_HttpHeaders& headers = response->GetHeaders();
    NPT_List<NPT_HttpHeader*>::Iterator header = headers.GetHeaders().GetFirstItem();
    while (header) {
        NPT_Debug("%s: %s\n", 
		  (const char*)(*header)->GetName(),
                  (const char*)(*header)->GetValue());
        ++header;
    }

    // show entity
    NPT_HttpEntity* entity = response->GetEntity();
    if (entity != NULL) {
        NPT_Debug("ENTITY: length=%d, type=%s, encoding=%s\n",
                  entity->GetContentLength(),
                  entity->GetContentType().GetChars(),
                  entity->GetContentEncoding().GetChars());
    }

    // dump the body
    NPT_InputStreamReference stream;
    entity->GetInputStream(stream);
    NPT_OutputStreamReference output;
    NPT_File standard_out(NPT_FILE_STANDARD_OUTPUT);
    standard_out.Open(NPT_FILE_OPEN_MODE_WRITE);
    standard_out.GetOutputStream(output);
    NPT_Size bytes_read;
    char buffer[1024];
    while (NPT_SUCCEEDED(stream->Read(buffer, sizeof(buffer), &bytes_read))) {
        output->Write(buffer, bytes_read);
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
|       TestHttpPost
+---------------------------------------------------------------------*/
static void 
TestHttpPost(const char* arg)
{
    NPT_HttpUrl url(arg);
    NPT_HttpRequest request(url, NPT_HTTP_METHOD_POST);
    NPT_HttpClient client;
    NPT_HttpResponse* response;

    NPT_HttpEntity* body_entity = new NPT_HttpEntity();
    NPT_InputStreamReference body_stream(new NPT_MemoryStream((void*)"hello blabla", 12));
    body_entity->SetInputStream(body_stream);

    request.SetEntity(body_entity);
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

    // check args
#if 1
    if (argc != 2) {
        NPT_Debug("HttpClient: missing URL argument\n");
        return -1;
    }
#endif

#if 1
    // test URL parsing
    TestUrlParser(NULL);
    TestUrlParser("");
    TestUrlParser("http");
    TestUrlParser("http:/");
    TestUrlParser("http://");
    TestUrlParser("http://foo");
    TestUrlParser("http://foo.bar");
    TestUrlParser("http://foo.bar:");
    TestUrlParser("http://foo.bar:1");
    TestUrlParser("http://foo.bar:176");
    TestUrlParser("http://foo.bar:176a");
    TestUrlParser("http://foo.bar:176/");
    TestUrlParser("http://foo.bar:176/blabla");
    TestUrlParser("http://foo.bar:176/blabla/blibli");
    TestUrlParser("http://foo.bar:176/blabla/blibli/");
    TestUrlParser("http://foo.bar/");
    TestUrlParser("http://foo.bar/blabla");
#endif

    TestHttpGet(argv[1]);
    TestHttpPost(argv[1]);

#if defined(WIN32) && defined(_DEBUG)
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}
