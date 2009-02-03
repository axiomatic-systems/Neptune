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

bool KillRequest = false;

/*----------------------------------------------------------------------
|       KillHandler
+---------------------------------------------------------------------*/
class KillHandler : public NPT_HttpRequestHandler
{
public:
    NPT_Result SetupResponse(NPT_HttpRequest&              /*request*/, 
                             const NPT_HttpRequestContext& /*context*/,
                             NPT_HttpResponse&             response) {
        NPT_HttpEntity* entity = response.GetEntity();
        entity->SetContentType("text/html");
        entity->SetInputStream("<html><body>Bye Bye!</body></html>");
        KillRequest = true;
        
        return NPT_SUCCESS;
    }
};

/*----------------------------------------------------------------------
|       TestHandler1
+---------------------------------------------------------------------*/
class TestHandler1 : public NPT_HttpRequestHandler
{
public:
    NPT_Result SetupResponse(NPT_HttpRequest&              request, 
                             const NPT_HttpRequestContext& context,
                             NPT_HttpResponse&             response) {
        NPT_String msg = "<HTML>";
        msg += "PATH=";
        msg += request.GetUrl().GetPath();
        msg += "<P><B>Local Address:</B> ";
        msg += context.GetLocalAddress().ToString();
        msg += "<P>";
        msg += "<B>Remote Address:</B> ";
        msg += context.GetRemoteAddress().ToString();
        msg += "<P><UL>";
        if (request.GetUrl().HasQuery()) {
            NPT_UrlQuery query(request.GetUrl().GetQuery());
            for (NPT_List<NPT_UrlQuery::Field>::Iterator it = query.GetFields().GetFirstItem();
                 it;
                 ++it) {
                 NPT_UrlQuery::Field& field = *it;
                 msg += "<LI>";
                 msg += field.m_Name;
                 msg += " = ";
                 msg += field.m_Value;
                 msg += " </LI>";
                 
                 // check for a 'delay' field
                 if (field.m_Name == "delay") {
                    NPT_UInt32 delay = 0;
                    field.m_Value.ToInteger(delay);
                    NPT_Debug("DELAY: %d seconds\n", delay);
                    NPT_System::Sleep(NPT_TimeInterval((float)delay));
                 }
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
|       TestHandler2
+---------------------------------------------------------------------*/
class TestHandler2 : public NPT_HttpRequestHandler
{
public:
    NPT_Result SetupResponse(NPT_HttpRequest&              /*request*/, 
                             const NPT_HttpRequestContext& /*context*/,
                             NPT_HttpResponse&             response) {
        NPT_HttpEntity* entity = response.GetEntity();
        entity->SetContentType("text/html");
        return NPT_SUCCESS;
    }
    
    NPT_Result SendResponseBody(const NPT_HttpRequestContext& /*context*/,
                                NPT_HttpResponse&             /*response*/,
                                NPT_OutputStream&             output) {
        output.WriteString("<html><body>\r\n");
        for (unsigned int i=0; i<100; i++) {
            output.WriteString("Line ");
            output.WriteString(NPT_String::FromInteger(i).GetChars());
            output.Flush();
            NPT_System::Sleep(NPT_TimeInterval(1.0f));
        }
        output.WriteString("</body></html>\r\n");
        return NPT_SUCCESS;
    }
};

/*----------------------------------------------------------------------
|       TestHttp
+---------------------------------------------------------------------*/
static NPT_Result 
TestHttp()
{
    NPT_HttpServer            server(1234);
    NPT_InputStreamReference  input;
    NPT_OutputStreamReference output;
    NPT_HttpRequestContext    context;

    NPT_HttpStaticRequestHandler* static_handler = new NPT_HttpStaticRequestHandler(
        "<html><body>"
        "<h1>Neptune HTTP Server Test 1</h1>"
        "<a href='/files-autodir'>List files working directory (autodir)</a><br>"
        "<a href='/files'>List files working directory (no autodir)</a><br>"
        "<a href='/test1'>Test 1</a><br>"
        "<a href='/test2'>Test 2</a><br>"
        "<a href='/kill'>Kill Test Server</a><br>"
        "</body></html>", 
        "text/html");
    server.AddRequestHandler(static_handler, "/", false);

    KillHandler* kill_handler = new KillHandler();
    server.AddRequestHandler(kill_handler, "/kill", false);

    TestHandler1* test_handler1 = new TestHandler1();
    server.AddRequestHandler(test_handler1, "/test1", false);

    TestHandler2* test_handler2 = new TestHandler2();
    server.AddRequestHandler(test_handler2, "/test2", false);

    NPT_String cwd;
    NPT_File::GetWorkingDirectory(cwd);
    NPT_HttpFileRequestHandler* file_handler_autodir = new NPT_HttpFileRequestHandler("/files-autodir", cwd.GetChars(), true);
    server.AddRequestHandler(file_handler_autodir, "/files-autodir", true);
    NPT_HttpFileRequestHandler* file_handler_noautodir = new NPT_HttpFileRequestHandler("/files", cwd.GetChars(), false);
    server.AddRequestHandler(file_handler_noautodir, "/files", true);

    do {
        NPT_Console::Output("Test HTTP server waiting for connection on port 1234...\n");
        NPT_Result result = server.WaitForNewClient(input, 
                                                    output,
                                                    &context);
        NPT_Console::OutputF("WaitForNewClient returned %d (%s)\n", result, NPT_ResultText(result));
        if (NPT_FAILED(result)) return result;

        result = server.RespondToClient(input, output, context);
        NPT_Console::OutputF("RespondToClient returned %d (%s)\n", result, NPT_ResultText(result));
        
        input = NULL;
        output = NULL;
    } while (!KillRequest);

    NPT_Console::OutputF("Killing Server\n");
    
    delete static_handler;
    delete test_handler1;
    delete test_handler2;
    delete file_handler_autodir;
    delete file_handler_noautodir;
    
    return NPT_SUCCESS;
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
