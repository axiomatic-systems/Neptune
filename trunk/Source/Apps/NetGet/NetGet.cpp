/*****************************************************************
|
|      Neptune Utilities - Network 'Get' Client
|
|      (c) 2001-2010 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include <stdio.h>

/*----------------------------------------------------------------------
|       PrintUsageAndExit
+---------------------------------------------------------------------*/
static void
PrintUsageAndExit(void)
{
    fprintf(stderr, 
            "NetGet [options] <url>\n"
            "\n"
            "  Options:\n"
            "    --verbose    : print verbose information\n"
            "    --show-proxy : show the proxy that will be used for the connection\n");
}

/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int
main(int argc, char** argv)
{
    // check command line
    if (argc < 2) {
        PrintUsageAndExit();
        return 1;
    }

    // init options
    bool verbose = false;
    bool show_proxy = false;
    bool url_set = false;
    NPT_HttpUrl url;
    
    // parse command line
    ++argv;
    const char* arg;
    while ((arg = *argv++)) {
        if (NPT_StringsEqual(arg, "--verbose")) {
            verbose = true;
        } else if (NPT_StringsEqual(arg, "--show-proxy")) {
            show_proxy = true;
        } else if (!url_set) {
            NPT_Result result = url.Parse(arg);
            if (NPT_FAILED(result)) {
                fprintf(stderr, "ERROR: failed to parse URL (%d:%s)\n", result, NPT_ResultText(result));
                return 1;
            }
            url_set = true;
        } else {
            fprintf(stderr, "ERROR: unexpected argument '%s'\n", arg);
            return 1;
        }
    }

    if (show_proxy) {
        NPT_HttpProxyAddress proxy;
        NPT_HttpProxySelector* selector = NPT_HttpProxySelector::GetDefault();
        if (selector) {
            NPT_Result result = selector->GetProxyForUrl(url, proxy);
            if (NPT_FAILED(result) && result != NPT_ERROR_HTTP_NO_PROXY) {
                fprintf(stderr, "ERROR: proxy selector error (%d:%s)\n", result, NPT_ResultText(result));
                return 1;
            }
        } 
        if (proxy.GetHostName().IsEmpty()) {
            printf("PROXY: none\n");
        } else {
            printf("PROXY: %s:%d\n", proxy.GetHostName().GetChars(), proxy.GetPort());
        }
    }
    
    // get the document
    NPT_HttpRequest request(url, NPT_HTTP_METHOD_GET);
    NPT_HttpClient client;
    NPT_HttpResponse* response;
    NPT_Result result = client.SendRequest(request, response);
    if (NPT_FAILED(result)) {
        fprintf(stderr, "ERROR: SendRequest failed (%d:%s)\n", result, NPT_ResultText(result));
        return 1;
    }

    // show response info
    if (verbose) {
        printf("#RESPONSE: protocol=%s, code=%d, reason=%s\n",
               response->GetProtocol().GetChars(),
               response->GetStatusCode(),
               response->GetReasonPhrase().GetChars());

        // show headers
        NPT_HttpHeaders& headers = response->GetHeaders();
        NPT_List<NPT_HttpHeader*>::Iterator header = headers.GetHeaders().GetFirstItem();
        while (header) {
            printf("%s: %s\n", 
                      (const char*)(*header)->GetName(),
                      (const char*)(*header)->GetValue());
            ++header;
        }
    }
    
    // show entity
    NPT_HttpEntity* entity = response->GetEntity();
    if (entity != NULL) {
        if (verbose) {
            printf("#ENTITY: length=%lld, type=%s, encoding=%s\n",
                   entity->GetContentLength(),
                   entity->GetContentType().GetChars(),
                   entity->GetContentEncoding().GetChars());
        }
        
        NPT_DataBuffer body;
        result =entity->Load(body);
        if (NPT_FAILED(result)) {
            fprintf(stderr, "ERROR: failed to load entity (%d)\n", result);
        } else {
            if (verbose) printf("#BODY: loaded %d bytes\n", (int)body.GetDataSize());

            // dump the body
            NPT_OutputStreamReference output;
            NPT_File standard_out(NPT_FILE_STANDARD_OUTPUT);
            standard_out.Open(NPT_FILE_OPEN_MODE_WRITE);
            standard_out.GetOutputStream(output);
            output->Write(body.GetData(), body.GetDataSize());
        }
    }

    delete response;
    
    return 0;
}




