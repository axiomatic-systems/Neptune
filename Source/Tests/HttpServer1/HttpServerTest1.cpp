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
|       TestHandler
+---------------------------------------------------------------------*/
class TestHandler : public NPT_HttpRequestHandler
{
public:
    NPT_Result SetupResponse(NPT_HttpRequest&  request, 
                             NPT_HttpResponse& response) {
        NPT_String msg = "<HTML>";
        msg += "PATH=";
        msg += request.GetUrl().GetPath();
        msg += " <P><UL>";
        if (request.GetUrl().HasQuery()) {
            NPT_HttpUrlQuery query(request.GetUrl().GetQuery());
            for (NPT_List<NPT_HttpUrlQuery::Field>::Iterator it = query.GetFields().GetFirstItem();
                 it;
                 ++it) {
                 NPT_HttpUrlQuery::Field& field = *it;
                 msg += "<LI>";
                 msg += field.m_Name;
                 msg += " = ";
                 msg += field.m_Value;
                 msg += " </LI>";
            }
        }
        msg += "</UL></HTML>";

        if (request.GetMethod() == NPT_HTTP_METHOD_POST) {
            NPT_DataBuffer request_body;
            request.GetEntity()->Load(request_body);
            NPT_Debug("REQUEST: body = %d bytes\n", request_body.GetDataSize());
            NPT_Debug("REQUEST: content type = %s\n", request.GetEntity()->GetContentType().GetChars());
            if (request.GetEntity()->GetContentType().StartsWith("text") ||
                request.GetEntity()->GetContentType() == "application/x-www-form-urlencoded") {
                NPT_String body_string;
                body_string.Assign((char*)request_body.GetData(), request_body.GetDataSize());
                NPT_Debug("%s", body_string.GetChars());
            }
        }

        NPT_HttpEntity* entity = response.GetEntity();
        entity->SetContentType("text/html");
        entity->SetInputStream(msg);

        return NPT_SUCCESS;
    }
};

/*----------------------------------------------------------------------
|       TestHttp
+---------------------------------------------------------------------*/
static NPT_Result 
TestHttp()
{
    NPT_HttpServer            server;
    NPT_InputStreamReference  input;
    NPT_OutputStreamReference output;
    NPT_SocketAddress         local_address;

    NPT_HttpBufferRequestHandler* handler1 = new NPT_HttpBufferRequestHandler("<HTML><H1>Hello World</H1></HTML>", "text/html");
    server.AddRequestHandler(handler1, "/test", false);

    TestHandler* test_handler = new TestHandler();
    server.AddRequestHandler(test_handler, "/test2", false);

    NPT_Result result = server.WaitForNewClient(input, 
                                                output,
                                                &local_address,
                                                NULL);
    NPT_Debug("WaitForNewClient returned %d\n", result);
    if (NPT_FAILED(result)) return result;

    result = server.RespondToClient(input, output, &local_address);
    NPT_Debug("ResponToClient returned %d\n", result);

    delete handler1;

    return result;
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

    while (NPT_SUCCEEDED(TestHttp()));

#if defined(WIN32) && defined(_DEBUG)
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}
