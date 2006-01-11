/*****************************************************************
|
|      Neptune - URI
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "NptUri.h"
#include "NptUtils.h"

/*----------------------------------------------------------------------
|       NPT_Uri::NPT_Uri
+---------------------------------------------------------------------*/
NPT_Uri::NPT_Uri(const char* uri)
{
    if (uri == NULL) return;

    // parse the scheme
    const char* scheme = uri;
    while (*scheme) {
        if (*scheme == ':') {
            m_Scheme.Assign(uri, (NPT_Size)(scheme-uri));
            m_Scheme.MakeLowercase();
            m_Specific = scheme+1;

            // compute the scheme id
            if (m_Scheme == "http") {
                m_SchemeId = SCHEME_ID_HTTP;
            } else {
                m_SchemeId = SCHEME_ID_UNKNOWN;
            }
            return;
        }
        scheme++;
    }
}

/*----------------------------------------------------------------------
|       NPT_Uri::Encode
+---------------------------------------------------------------------*/
NPT_String
NPT_Uri::Encode(const char* uri)
{
    NPT_String encoded;

    // check args
    if (uri == NULL) return encoded;

    // reserve at least the size of the current uri
    encoded.Reserve(NPT_StringLength(uri));

    // process each character
    while (unsigned char c = *uri++) {
        if (c == '<'  ||
            c == '>'  || 
            c == '"'  || 
            c == '{'  || 
            c == '}'  || 
            c == '|'  ||
            c == '\\' ||
            c == '^'  ||
            c == '\'' || 
            c < 0x20  ||
            c > 0x7F) {
            // needs to be escaped
            char escaped[3];
            escaped[0] = '%';
            NPT_ByteToHex(c, &escaped[1]);
            encoded.Append(escaped, 3);
        } else {
            // no escaping required
            encoded += c;
        }
    }
    
    return encoded;
}
