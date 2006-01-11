/*****************************************************************
|
|      Neptune - HTTP Protocol
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "NptHttp.h"
#include "NptSockets.h"
#include "NptBufferedStreams.h"
#include "NptDebug.h"
#include "NptVersion.h"

/*----------------------------------------------------------------------
|       types
+---------------------------------------------------------------------*/
typedef enum {
    NPT_HTTP_URL_PARSER_STATE_START,
    NPT_HTTP_URL_PARSER_STATE_LEADING_SLASH,
    NPT_HTTP_URL_PARSER_STATE_HOST,
    NPT_HTTP_URL_PARSER_STATE_PORT
} NPT_HttpUrlParserState;

/*----------------------------------------------------------------------
|       NPT_HttpUrl::NPT_HttpUrl
+---------------------------------------------------------------------*/
NPT_HttpUrl::NPT_HttpUrl(const char* url) : 
    NPT_Uri(url),
    m_Port(NPT_HTTP_INVALID_PORT),
    m_Path("/")
{
    // check parameters
    if (url == NULL) return;

    // intialize the parser
    NPT_HttpUrlParserState state = NPT_HTTP_URL_PARSER_STATE_START;
    url = m_Specific;
    const char* mark = url;

    // parse the URL
    char c;
    do  {
        c = *url++;
        switch (state) {
          case NPT_HTTP_URL_PARSER_STATE_START:
            if (c == '/') {
                state = NPT_HTTP_URL_PARSER_STATE_LEADING_SLASH;
            } else {
                return;
            }
            break;

          case NPT_HTTP_URL_PARSER_STATE_LEADING_SLASH:
            if (c == '/') {
                state = NPT_HTTP_URL_PARSER_STATE_HOST;
                mark = url;
            } else {
                return;
            }
            break;

          case NPT_HTTP_URL_PARSER_STATE_HOST:
            if (c == ':') {
                m_Host.Assign(mark, (NPT_Size)(url-mark));
                state = NPT_HTTP_URL_PARSER_STATE_PORT;
            } else if (c == '/' || c == 0) {
                m_Host.Assign(mark, (NPT_Size)(url-1-mark));
                m_Port = NPT_HTTP_DEFAULT_PORT;
                if (c == '/') m_Path += url;
                return;
            }
            break;

          case NPT_HTTP_URL_PARSER_STATE_PORT:
            if (c >= '0' && c <= '9') {
                m_Port = m_Port*10+(c-'0');
            } else if (c == '/' || c == 0) {
                if (c == '/') m_Path += url;
                return;
            } else {
                // invalid character
                m_Port = NPT_HTTP_INVALID_PORT;
                return;
            }
        }
    } while (c);
}

/*----------------------------------------------------------------------
|       NPT_HttpHeader::NPT_HttpHeader
+---------------------------------------------------------------------*/
NPT_HttpHeader::NPT_HttpHeader(const char* name, const char* value):
    m_Name(name), 
    m_Value(value)
{
}

/*----------------------------------------------------------------------
|       NPT_HttpHeader::~NPT_HttpHeader
+---------------------------------------------------------------------*/
NPT_HttpHeader::~NPT_HttpHeader()
{
}

/*----------------------------------------------------------------------
|       NPT_HttpHeader::Emit
+---------------------------------------------------------------------*/
NPT_Result
NPT_HttpHeader::Emit(NPT_OutputStream& stream) const
{
    stream.WriteString(m_Name);
    stream.Write(": ", 2);
    stream.WriteString(m_Value);
    stream.Write(NPT_HTTP_LINE_TERMINATOR, 2);

    NPT_Debug("%s: %s\n", (const char*)m_Name, (const char*)m_Value);

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_HttpHeader::SetName
+---------------------------------------------------------------------*/
NPT_Result 
NPT_HttpHeader::SetName(const char* name)
{
    m_Name = name;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_HttpHeader::~NPT_HttpHeader
+---------------------------------------------------------------------*/
NPT_Result 
NPT_HttpHeader::SetValue(const char* value)
{
    m_Value = value;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_HttpHeaders::NPT_HttpHeaders
+---------------------------------------------------------------------*/
NPT_HttpHeaders::NPT_HttpHeaders()
{
}

/*----------------------------------------------------------------------
|       NPT_HttpHeaders::~NPT_HttpHeaders
+---------------------------------------------------------------------*/
NPT_HttpHeaders::~NPT_HttpHeaders()
{
}

/*----------------------------------------------------------------------
|       NPT_HttpHeaders::Emit
+---------------------------------------------------------------------*/
NPT_Result
NPT_HttpHeaders::Emit(NPT_OutputStream& stream) const
{
    // for each header in the list
    NPT_List<NPT_HttpHeader>::Item* header = m_Headers.GetFirstItem();
    while (header != NULL) {
        // emit the header
        NPT_Result result = header->GetData().Emit(stream);
        if (NPT_FAILED(result)) return result;

        header = header->GetNext();
    }
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_HttpHeaders::GetHeader
+---------------------------------------------------------------------*/
NPT_HttpHeader*
NPT_HttpHeaders::GetHeader(const char* name) const
{
    // check args
    if (name == NULL) return NULL;

    // find a matching header
    NPT_List<NPT_HttpHeader>::Item* header = m_Headers.GetFirstItem();
    while (header != NULL) {
        if (header->GetData().GetName().Compare(name, true) == 0) {
            return &header->GetData();
        }
        header = header->GetNext();
    }

    // not found
    return NULL;
}

/*----------------------------------------------------------------------
|       NPT_HttpHeaders::AddHeader
+---------------------------------------------------------------------*/
NPT_Result
NPT_HttpHeaders::AddHeader(const char* name, const char* value)
{
    m_Headers.Add(NPT_HttpHeader(name, value));
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_HttpHeaders::SetHeader
+---------------------------------------------------------------------*/
NPT_Result
NPT_HttpHeaders::SetHeader(const char* name, const char* value)
{
    NPT_HttpHeader* header = GetHeader(name);
    if (header == NULL) {
        return AddHeader(name, value);
    } else {
        return header->SetValue(value);
    }
}

/*----------------------------------------------------------------------
|       NPT_HttpEntity::NPT_HttpEntity
+---------------------------------------------------------------------*/
NPT_HttpEntity::NPT_HttpEntity() :
    m_ContentLength(0)
{
}

/*----------------------------------------------------------------------
|       NPT_HttpEntity::NPT_HttpEntity
+---------------------------------------------------------------------*/
NPT_HttpEntity::NPT_HttpEntity(const NPT_HttpHeaders& headers) :
    m_ContentLength(0)
{
    NPT_HttpHeader* header;
    
    // Content-Length
    header = headers.GetHeader(NPT_HTTP_HEADER_CONTENT_LENGTH);
    if (header != NULL) {
        long length;
        if (NPT_SUCCEEDED(header->GetValue().ToInteger(length))) {
            m_ContentLength = length;
        } else {
            m_ContentLength = 0;
        }
    }

    // Content-Type
    header = headers.GetHeader(NPT_HTTP_HEADER_CONTENT_TYPE);
    if (header != NULL) {
        m_ContentType = header->GetValue();
    }

    // Content-Encoding
    header = headers.GetHeader(NPT_HTTP_HEADER_CONTENT_ENCODING);
    if (header != NULL) {
        m_ContentEncoding = header->GetValue();
    }
}

/*----------------------------------------------------------------------
|       NPT_HttpEntity::~NPT_HttpEntity
+---------------------------------------------------------------------*/
NPT_HttpEntity::~NPT_HttpEntity()
{
}

/*----------------------------------------------------------------------
|       NPT_HttpEntity::GetInputStream
+---------------------------------------------------------------------*/
NPT_Result 
NPT_HttpEntity::GetInputStream(NPT_BufferedInputStreamReference& stream)
{
    stream = m_InputStream;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_HttpEntity::SetInputStream
+---------------------------------------------------------------------*/
NPT_Result 
NPT_HttpEntity::SetInputStream(const NPT_BufferedInputStreamReference& stream)
{
    m_InputStream = stream;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_HttpMessage::NPT_HttpMessage
+---------------------------------------------------------------------*/
NPT_HttpMessage::NPT_HttpMessage(const char* protocol) :
    m_Protocol(protocol),
    m_Entity(NULL)
{
}

/*----------------------------------------------------------------------
|       NPT_HttpMessage::NPT_HttpMessage
+---------------------------------------------------------------------*/
NPT_HttpMessage::~NPT_HttpMessage()
{
    delete m_Entity;
}

/*----------------------------------------------------------------------
|       NPT_HttpMessage::SetEntity
+---------------------------------------------------------------------*/
NPT_Result
NPT_HttpMessage::SetEntity(NPT_HttpEntity* entity)
{
    delete m_Entity;
    m_Entity = entity;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_HttpRequest::NPT_HttpRequest
+---------------------------------------------------------------------*/
NPT_HttpRequest::NPT_HttpRequest(const NPT_HttpUrl& url, 
                                 const char*        method, 
                                 const char*        protocol) :
    NPT_HttpMessage(protocol),
    m_Url(url),
    m_Method(method)
{
}

/*----------------------------------------------------------------------
|       NPT_HttpRequest::~NPT_HttpRequest
+---------------------------------------------------------------------*/
NPT_HttpRequest::~NPT_HttpRequest()
{
}

/*----------------------------------------------------------------------
|       NPT_HttpRequest::Emit
+---------------------------------------------------------------------*/
NPT_Result
NPT_HttpRequest::Emit(NPT_OutputStream& stream) const
{
    NPT_Debug("NPT_HttpRequest::Emit ++\n");
    // write the request line
    stream.WriteString(m_Method);
    stream.Write(" ", 1);
    stream.WriteString(NPT_Uri::Encode(m_Url.GetPath()));
    stream.Write(" ", 1);
    stream.WriteString(m_Protocol);
    stream.Write(NPT_HTTP_LINE_TERMINATOR, 2);

    NPT_Debug("%s %s %s\n", 
	      (const char*)m_Method, 
	      (const char*)NPT_Uri::Encode(m_Url.GetPath()), 
	      (const char*)m_Protocol);

    // emit headers
    m_Headers.Emit(stream);

    // finish with an empty line
    stream.Write(NPT_HTTP_LINE_TERMINATOR, 2);

    NPT_Debug("NPT_HttpRequest::Emit --\n");

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_HttpResponse::NPT_HttpResponse
+---------------------------------------------------------------------*/
NPT_HttpResponse::NPT_HttpResponse(NPT_HttpStatusCode status_code,
                                   const char*        reason_phrase,
                                   const char*        protocol) :
    NPT_HttpMessage(protocol),
    m_StatusCode(status_code),
    m_ReasonPhrase(reason_phrase)
{
}

/*----------------------------------------------------------------------
|       NPT_HttpResponse::~NPT_HttpResponse
+---------------------------------------------------------------------*/
NPT_HttpResponse::~NPT_HttpResponse()
{
}

/*----------------------------------------------------------------------
|       NPT_HttpResponse::Parse
+---------------------------------------------------------------------*/
NPT_Result
NPT_HttpResponse::Parse(NPT_BufferedInputStream& stream, 
                        NPT_HttpResponse*&       response, 
                        NPT_Timeout             /*timeout*/)
{
    NPT_Result result;

    // default return value
    response = NULL;

    // read the response line
    NPT_String line;
    result = stream.ReadLine(line, NPT_HTTP_PROTOCOL_MAX_LINE_LENGTH);
    if (NPT_FAILED(result)) return result;
    
    // check the response line
    int first_space = line.Find(' ');
    if (first_space < 0) return NPT_ERROR_HTTP_INVALID_RESPONSE_LINE;
    int second_space = line.Find(' ', first_space+1);
    if (second_space < 0 || second_space-first_space != 4) {
        return NPT_ERROR_HTTP_INVALID_RESPONSE_LINE;
    }

    // parse the response line
    NPT_String protocol = line.SubString(0, first_space);
    NPT_String status_code = line.SubString(first_space+1, 3);
    NPT_String reason_phrase = line.SubString(first_space+1+3+1, 
                                              line.GetLength()-(first_space+1+3+1));

    // create a response object
    long status_code_int = 0;
    status_code.ToInteger(status_code_int);
    response = new NPT_HttpResponse(status_code_int, reason_phrase, protocol);

    // parse headers
    NPT_HttpHeaders& headers = response->GetHeaders();
    NPT_String       header_name;
    NPT_String       header_value;
    bool             header_pending = false;
    while (NPT_SUCCEEDED(stream.ReadLine(line, NPT_HTTP_PROTOCOL_MAX_LINE_LENGTH))) {
        if (line.GetLength() == 0) {
            // empty line, end of headers
            if (header_pending) {
                header_value.Trim();
                headers.AddHeader(header_name, header_value);
            }
            break;
        }
        char first_char = line[0];
        if ((first_char == ' ' || first_char == '\t') && header_pending) {
            header_value.Append(line.GetChars()+1, line.GetLength()-1);
        } else {
            // add the pending header to the list
            if (header_pending) {
                header_value.Trim();
                headers.AddHeader(header_name, header_value);
            }

            // find the colon separating the name and the value
            int colon_index = line.Find(':');
            if (colon_index < 1) {
                // invalid syntax, ignore
                continue;
            }
            header_name = line.Left(colon_index);

            // the field value starts at the first non-whitespace
            const char* value = line.GetChars()+colon_index+1;
            while (*value == ' ' || *value == '\t') {
                value++;
            }
            header_value = value;
           
            // the header is pending
            header_pending = true;
        }
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_HttpClient::NPT_HttpClient
+---------------------------------------------------------------------*/
NPT_HttpClient::NPT_HttpClient()
{
}

/*----------------------------------------------------------------------
|       NPT_HttpClient::~NPT_HttpClient
+---------------------------------------------------------------------*/
NPT_HttpClient::~NPT_HttpClient()
{
}

/*----------------------------------------------------------------------
|       NPT_HttpClient::SendRequest
+---------------------------------------------------------------------*/
NPT_Result
NPT_HttpClient::SendRequest(NPT_HttpRequest&   request, 
                            NPT_HttpResponse*& response,
                            NPT_Timeout        timeout)
{
    NPT_Result result;

    // setup default values
    response = NULL;

    // get the address of the server
    NPT_IpAddress server_address;
    result = server_address.ResolveName(request.GetUrl().GetHost(), timeout);
    if (NPT_FAILED(result)) return result;
    NPT_SocketAddress address(server_address, request.GetUrl().GetPort());

    // connect to the server
    NPT_TcpClientSocket connection;
    result = connection.Connect(address, timeout);
    if (NPT_FAILED(result)) return result;

    // get the socket stream to send the request
    NPT_OutputStreamReference output_stream;
    result = connection.GetOutputStream(output_stream);
    if (NPT_FAILED(result)) return result;

    // add any headers that may be missing
    request.GetHeaders().SetHeader(NPT_HTTP_HEADER_CONNECTION, "close");
    request.GetHeaders().SetHeader(NPT_HTTP_HEADER_HOST, request.GetUrl().GetHost());
    request.GetHeaders().SetHeader(NPT_HTTP_HEADER_USER_AGENT, 
                                   "Neptune/" NPT_NEPTUNE_VERSION_STRING);

    // create a buffered stream for this socket stream
    //m_ConnectionStream = new NPT_BufferedByteStream(stream);

    NPT_Debug("SendRequest: will connect to %s:%d\n", (const char*)request.GetUrl().GetHost(), request.GetUrl().GetPort());
    request.Emit(*output_stream);

    // flush the output stream so that everything is sent to the server
    output_stream->Flush();

    // get the input stream to read the response
    NPT_InputStreamReference input_stream;
    result = connection.GetInputStream(input_stream);
    if (NPT_FAILED(result)) return result;

    // create a buffered stream for this socket stream
    NPT_BufferedInputStream* buffered_input_stream = new NPT_BufferedInputStream(input_stream);

    // parse the response
    result = NPT_HttpResponse::Parse(*buffered_input_stream, response, timeout);
    if (NPT_FAILED(result)) return result;

    // decide what to do next based on the response 
    //switch (response->GetStatusCode()) {
    //    // redirections
    //    case NPT_HTTP_STATUS_301_MOVED_PERMANENTLY:
    //    case NPT_HTTP_STATUS_302_FOUND:
    //}

    // create an entity if one is expected in the response
    if (request.GetMethod() == NPT_HTTP_METHOD_GET) {
        NPT_HttpEntity* entity = new NPT_HttpEntity(response->GetHeaders());
        NPT_BufferedInputStreamReference isr(buffered_input_stream);
        entity->SetInputStream(isr);
        response->SetEntity(entity);
    }

    // unbuffer the stream 
    buffered_input_stream->SetBufferSize(0);

    return result;
}
