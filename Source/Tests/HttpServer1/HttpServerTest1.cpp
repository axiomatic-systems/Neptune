/*****************************************************************
|
|      HTTP Server Test Program 1
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

/*----------------------------------------------------------------------
|       ShowRequest
+---------------------------------------------------------------------*/
static void
ShowRequest(NPT_HttpRequest* request)
{
    // show response info
    NPT_Debug("REQUEST: url=%s, protocol=%s\n",
              request->GetUrl().ToString().GetChars(),
              request->GetProtocol().GetChars());

    // show headers
    NPT_HttpHeaders& headers = request->GetHeaders();
    NPT_List<NPT_HttpHeader*>::Iterator header = headers.GetHeaders().GetFirstItem();
    while (header) {
        NPT_Debug("%s: %s\n", 
          (const char*)(*header)->GetName(),
          (const char*)(*header)->GetValue());
        ++header;
    }

    // show entity
    NPT_HttpEntity* entity = request->GetEntity();
    if (entity != NULL) {
        NPT_Debug("ENTITY: length=%d, type=%s, encoding=%s\n",
                  entity->GetContentLength(),
                  entity->GetContentType().GetChars(),
                  entity->GetContentEncoding().GetChars());
    }

    // dump the body
    if (entity && entity->GetContentLength()) {
        NPT_InputStreamReference input;
        entity->GetInputStream(input);
        NPT_OutputStreamReference output;
        NPT_File standard_out(NPT_FILE_STANDARD_OUTPUT);
        standard_out.Open(NPT_FILE_OPEN_MODE_WRITE);
        standard_out.GetOutputStream(output);
        NPT_StreamToStreamCopy(*input, *output);
    }
}

/*----------------------------------------------------------------------
|       TestHttp
+---------------------------------------------------------------------*/
static void 
TestHttp()
{
    NPT_HttpServer            server;
    NPT_HttpRequest*          request;
    NPT_InputStreamReference  input;
    NPT_OutputStreamReference output;
    NPT_SocketAddress         local_address;

    NPT_Result result = server.WaitForNewClient(input, 
                                                output,
                                                &local_address,
                                                NULL);
    NPT_Debug("WaitForNewClient returned %d\n", result);
    if (NPT_FAILED(result)) return;

    NPT_HttpResponder responder(input, output);
    result = responder.ParseRequest(request, &local_address);
    if (NPT_FAILED(result)) {
        NPT_Debug("ParseRequest failed\n");
    }

    ShowRequest(request);

    NPT_HttpResponse response(200, "Ok", NPT_HTTP_PROTOCOL_1_0);
    responder.SendResponse(response);

    delete request;
}

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

    TestHttp();

#if defined(WIN32) && defined(_DEBUG)
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}
