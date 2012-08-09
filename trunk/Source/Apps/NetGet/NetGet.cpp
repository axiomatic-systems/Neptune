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
            "    --verbose : print verbose information\n"
            "    --http-1-1 : use HTTP 1.1\n"
            "    --ssl-client-cert <filename> : load client TLS certificate from <filename> (PKCS12)\n"
            "    --ssl-client-cert-password <password> : optional password for the client cert\n"
            "    --ssl-accept-self-signed-certs : accept self-signed server certificates\n"
            "    --ssl-accept-hostname-mismatch : accept server certificates that don't match\n"
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
    bool verbose    = false;
    bool show_proxy = false;
    bool url_set    = false;
    bool http_1_1   = false;
    NPT_HttpUrl url;
    NPT_HttpClient::Connector* connector = NULL;
    NPT_TlsContext*    tls_context = NULL;
    const char*        tls_cert_filename = NULL;
    const char*        tls_cert_password = NULL;
    unsigned int       tls_options = 0;
    
    // parse command line
    ++argv;
    const char* arg;
    while ((arg = *argv++)) {
        if (NPT_StringsEqual(arg, "--verbose")) {
            verbose = true;
        } else if (NPT_StringsEqual(arg, "--show-proxy")) {
            show_proxy = true;
        } else if (NPT_StringsEqual(arg, "--http-1-1")) {
            http_1_1 = true;
        } else if (NPT_StringsEqual(arg, "--ssl-client-cert")) {
            tls_cert_filename = *argv++;
            if (tls_cert_filename == NULL) {
                fprintf(stderr, "ERROR: missing argument after --ssl-client-cert option\n");
                return 1;
            }
        } else if (NPT_StringsEqual(arg, "--ssl-client-cert-password")) {
            tls_cert_password = *argv++;
            if (tls_cert_password == NULL) {
                fprintf(stderr, "ERROR: missing argument after --ssl-client-cert-password option\n");
                return 1;
            }
        } else if (NPT_StringsEqual(arg, "--ssl-accept-self-signed-certs")) {
            tls_options |= NPT_HttpTlsConnector::OPTION_ACCEPT_SELF_SIGNED_CERTS;
        } else if (NPT_StringsEqual(arg, "--ssl-accept-hostname-mismatch")) {
            tls_options |= NPT_HttpTlsConnector::OPTION_ACCEPT_HOSTNAME_MISMATCH;
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
    
    // load a client cert if needed
    if (tls_cert_filename) {
        NPT_DataBuffer cert;
        NPT_Result result = NPT_File::Load(tls_cert_filename, cert);
        if (NPT_FAILED(result)) {
            fprintf(stderr, "ERROR: failed to load client cert from file %s (%d)\n", tls_cert_filename, result);
            return 1;
        }
        tls_context = new NPT_TlsContext(NPT_TlsContext::OPTION_VERIFY_LATER | NPT_TlsContext::OPTION_ADD_DEFAULT_TRUST_ANCHORS);
        result = tls_context->LoadKey(NPT_TLS_KEY_FORMAT_PKCS12, cert.GetData(), cert.GetDataSize(), tls_cert_password);
        if (NPT_FAILED(result)) {
            fprintf(stderr, "ERROR: failed to parse client cert (%d)\n", result);
            return 1;
        }
        connector = new NPT_HttpTlsConnector(*tls_context, tls_options);
    }

    // get the document
    NPT_HttpRequest request(url, NPT_HTTP_METHOD_GET);
    NPT_HttpClient client;
    NPT_HttpResponse* response;
    if (http_1_1) {
        request.SetProtocol(NPT_HTTP_PROTOCOL_1_1);
    }
    if (connector) {
        client.SetConnector(connector);
    }
    NPT_Result result = client.SendRequest(request, response);
    if (NPT_FAILED(result)) {
        fprintf(stderr, "ERROR: SendRequest failed (%d:%s)\n", result, NPT_ResultText(result));
        return 1;
    }

    // show the request info
    if (verbose) {
        printf("#REQUEST: protocol=%s\n", request.GetProtocol().GetChars());

        // show headers
        NPT_HttpHeaders& headers = request.GetHeaders();
        NPT_List<NPT_HttpHeader*>::Iterator header = headers.GetHeaders().GetFirstItem();
        while (header) {
            printf("%s: %s\n", 
                      (const char*)(*header)->GetName(),
                      (const char*)(*header)->GetValue());
            ++header;
        }
    }

    // show response info
    if (verbose) {
        printf("\n#RESPONSE: protocol=%s, code=%d, reason=%s\n",
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
            printf("\n#ENTITY: length=%lld, type=%s, encoding=%s\n",
                   entity->GetContentLength(),
                   entity->GetContentType().GetChars(),
                   entity->GetContentEncoding().GetChars());
        }
        
        if (verbose) {
            NPT_InputStreamReference body_stream;
            entity->GetInputStream(body_stream);
            if (!body_stream.IsNull()) {
                NPT_LargeSize size;
                body_stream->GetSize(size);
                printf("Loading body stream (declared: %lld bytes)\n", size);
            }
        }
        NPT_DataBuffer body;
        result =entity->Load(body);
        if (NPT_FAILED(result)) {
            fprintf(stderr, "ERROR: failed to load entity (%d)\n", result);
        } else {
            if (verbose) printf("\n#BODY: loaded %d bytes\n", (int)body.GetDataSize());

            // dump the body
            NPT_OutputStreamReference output;
            NPT_File standard_out(NPT_FILE_STANDARD_OUTPUT);
            standard_out.Open(NPT_FILE_OPEN_MODE_WRITE);
            standard_out.GetOutputStream(output);
            output->Write(body.GetData(), body.GetDataSize());
        }
    }

    delete response;
    delete connector;
    delete tls_context;
    
    return 0;
}




