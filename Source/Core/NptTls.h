/*****************************************************************
|
|   Neptune - TLS/SSL Support
|
| Copyright (c) 2002-2008, Axiomatic Systems, LLC.
| All rights reserved.
|
| Redistribution and use in source and binary forms, with or without
| modification, are permitted provided that the following conditions are met:
|     * Redistributions of source code must retain the above copyright
|       notice, this list of conditions and the following disclaimer.
|     * Redistributions in binary form must reproduce the above copyright
|       notice, this list of conditions and the following disclaimer in the
|       documentation and/or other materials provided with the distribution.
|     * Neither the name of Axiomatic Systems nor the
|       names of its contributors may be used to endorse or promote products
|       derived from this software without specific prior written permission.
|
| THIS SOFTWARE IS PROVIDED BY AXIOMATIC SYSTEMS ''AS IS'' AND ANY
| EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
| WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
| DISCLAIMED. IN NO EVENT SHALL AXIOMATIC SYSTEMS BE LIABLE FOR ANY
| DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
| (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
| LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
| ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
| (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
| SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
|
 ****************************************************************/

#ifndef _NPT_TLS_H_
#define _NPT_TLS_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "NptConfig.h"
#include "NptStreams.h"
#include "NptTime.h"

/*----------------------------------------------------------------------
|   error codes
+---------------------------------------------------------------------*/
const NPT_Result NPT_ERROR_INVALID_PASSWORD                     = (NPT_ERROR_BASE_TLS-1);
const NPT_Result NPT_ERROR_TLS_INVALID_HANDSHAKE                = (NPT_ERROR_BASE_TLS-2);
const NPT_Result NPT_ERROR_TLS_INVALID_PROTOCOL_MESSAGE         = (NPT_ERROR_BASE_TLS-3);
const NPT_Result NPT_ERROR_TLS_INVALID_HMAC                     = (NPT_ERROR_BASE_TLS-4);
const NPT_Result NPT_ERROR_TLS_INVALID_VERSION                  = (NPT_ERROR_BASE_TLS-5);
const NPT_Result NPT_ERROR_TLS_INVALID_SESSION                  = (NPT_ERROR_BASE_TLS-6);
const NPT_Result NPT_ERROR_TLS_NO_CIPHER                        = (NPT_ERROR_BASE_TLS-7);
const NPT_Result NPT_ERROR_TLS_BAD_CERTIFICATE                  = (NPT_ERROR_BASE_TLS-8);
const NPT_Result NPT_ERROR_TLS_INVALID_KEY                      = (NPT_ERROR_BASE_TLS-9);
const NPT_Result NPT_ERROR_TLS_NO_CLIENT_RENEGOTIATION          = (NPT_ERROR_BASE_TLS-10);
const NPT_Result NPT_ERROR_TLS_INVALID_FINISHED_MESSAGE         = (NPT_ERROR_BASE_TLS-11);
const NPT_Result NPT_ERROR_TLS_NO_CERTIFICATE_DEFINED           = (NPT_ERROR_BASE_TLS-12);
const NPT_Result NPT_ERROR_TLS_ALERT_HANDSHAKE_FAILED           = (NPT_ERROR_BASE_TLS-13); 
const NPT_Result NPT_ERROR_TLS_ALERT_BAD_CERTIFICATE            = (NPT_ERROR_BASE_TLS-14);
const NPT_Result NPT_ERROR_TLS_ALERT_INVALID_VERSION            = (NPT_ERROR_BASE_TLS-15);
const NPT_Result NPT_ERROR_TLS_ALERT_BAD_RECORD_MAC             = (NPT_ERROR_BASE_TLS-16);
const NPT_Result NPT_ERROR_TLS_ALERT_DECODE_ERROR               = (NPT_ERROR_BASE_TLS-17);
const NPT_Result NPT_ERROR_TLS_ALERT_DECRYPT_ERROR              = (NPT_ERROR_BASE_TLS-18);
const NPT_Result NPT_ERROR_TLS_ALERT_ILLEGAL_PARAMETER          = (NPT_ERROR_BASE_TLS-19);
const NPT_Result NPT_ERROR_TLS_ALERT_UNEXPECTED_MESSAGE         = (NPT_ERROR_BASE_TLS-20);
const NPT_Result NPT_ERROR_TLS_CERTIFICATE_FAILURE              = (NPT_ERROR_BASE_TLS-21);
const NPT_Result NPT_ERROR_TLS_CERTIFICATE_NO_TRUST_ANCHOR      = (NPT_ERROR_BASE_TLS-22);
const NPT_Result NPT_ERROR_TLS_CERTIFICATE_BAD_SIGNATURE        = (NPT_ERROR_BASE_TLS-23);      
const NPT_Result NPT_ERROR_TLS_CERTIFICATE_NOT_YET_VALID        = (NPT_ERROR_BASE_TLS-24);
const NPT_Result NPT_ERROR_TLS_CERTIFICATE_EXPIRED              = (NPT_ERROR_BASE_TLS-25);
const NPT_Result NPT_ERROR_TLS_CERTIFICATE_SELF_SIGNED          = (NPT_ERROR_BASE_TLS-26);
const NPT_Result NPT_ERROR_TLS_CERTIFICATE_INVALID_CHAIN        = (NPT_ERROR_BASE_TLS-27);
const NPT_Result NPT_ERROR_TLS_CERTIFICATE_UNSUPPORTED_DIGEST   = (NPT_ERROR_BASE_TLS-28);
const NPT_Result NPT_ERROR_TLS_CERTIFICATE_INVALID_PRIVATE_KEY  = (NPT_ERROR_BASE_TLS-29);

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
const unsigned int NPT_TLS_NULL_WITH_NULL_NULL      = 0x00;
const unsigned int NPT_TLS_RSA_WITH_RC4_128_MD5     = 0x04;
const unsigned int NPT_TLS_RSA_WITH_RC4_128_SHA     = 0x05;
const unsigned int NPT_TLS_RSA_WITH_AES_128_CBC_SHA = 0x2F;
const unsigned int NPT_TLS_RSA_WITH_AES_256_CBC_SHA = 0x35;

/*----------------------------------------------------------------------
|   class references
+---------------------------------------------------------------------*/
class NPT_TlsContextImpl;
class NPT_TlsSessionImpl;

/*----------------------------------------------------------------------
|   types
+---------------------------------------------------------------------*/
typedef enum {
    NPT_TLS_KEY_FORMAT_RSA_PRIVATE,
    NPT_TLS_KEY_FORMAT_PKCS8,
    NPT_TLS_KEY_FORMAT_PKCS12
} NPT_TlsKeyFormat;

struct NPT_TlsTrustAnchorData {
    const unsigned char* cert_data;
    unsigned int         cert_size;
};

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
const unsigned int NPT_TLS_CONTEXT_OPTION_VERIFY_LATER               = 1;
const unsigned int NPT_TLS_CONTEXT_OPTION_REQUIRE_CLIENT_CERTIFICATE = 2;

/*----------------------------------------------------------------------
|   NPT_TlsContext
+---------------------------------------------------------------------*/
class NPT_TlsContext
{
public:
    NPT_TlsContext(NPT_Flags options=0);
   ~NPT_TlsContext();
   
    // methods
    NPT_Result LoadKey(NPT_TlsKeyFormat     key_format, 
                       const unsigned char* key_data,
                       NPT_Size             key_data_size,
                       const char*          password);
    NPT_Result AddTrustAnchor(const unsigned char* ta_data,
                              NPT_Size             ta_data_size);
    /**
     * Add one or more trust anchors, from a list
     * @param anchors Array of trust anchor data
     * @param anchor_count Number of anchors in the array, or 0 if the array
     * is terminated by a 'sentinel' (an anchor data with the field cert_data set
     * to NULL and the field cert_size set to 0).
     */
    NPT_Result AddTrustAnchors(const NPT_TlsTrustAnchorData* anchors, 
                               NPT_Cardinal                  anchor_count = 0);
      
    NPT_Result SelfSignCertificate(const char* common_name,
                                   const char* organization,
                                   const char* organizational_name);
                                   
protected:
    NPT_TlsContextImpl* m_Impl;
    
    // friends
    friend class NPT_TlsSession;
    friend class NPT_TlsClientSession;
    friend class NPT_TlsServerSession;
};

typedef NPT_Reference<NPT_TlsContext> NPT_TlsContextReference;

/*----------------------------------------------------------------------
|   NPT_TlsCertificateInfo
+---------------------------------------------------------------------*/
struct NPT_TlsCertificateInfo
{
    struct _subject {
        NPT_String common_name;
        NPT_String organization;
        NPT_String organizational_name;
    } subject;
    struct _issuer {
        NPT_String common_name;
        NPT_String organization;
        NPT_String organizational_name;
    } issuer;
    struct _fingerprint {
        unsigned char sha1[20];
        unsigned char md5[16];
    } fingerprint;
    NPT_DateTime issue_date;
    NPT_DateTime expiration_date;
    NPT_List<NPT_String> alternate_names;
};

/*----------------------------------------------------------------------
|   NPT_TlsSession
+---------------------------------------------------------------------*/
class NPT_TlsSession
{
public:
    virtual ~NPT_TlsSession();
    virtual NPT_Result Handshake();
    virtual NPT_Result GetHandshakeStatus();
    virtual NPT_Result GetPeerCertificateInfo(NPT_TlsCertificateInfo& info);
    virtual NPT_Result VerifyPeerCertificate();
    virtual NPT_Result GetSessionId(NPT_DataBuffer& session_id);
    virtual NPT_UInt32 GetCipherSuiteId();
    virtual NPT_Result GetInputStream(NPT_InputStreamReference& stream);
    virtual NPT_Result GetOutputStream(NPT_OutputStreamReference& stream);
    
protected:
    NPT_TlsSession(NPT_TlsContextReference& context, 
                   NPT_TlsSessionImpl*      impl);

    NPT_TlsContextReference           m_Context;
    NPT_Reference<NPT_TlsSessionImpl> m_Impl;
    NPT_InputStreamReference          m_InputStream;
    NPT_OutputStreamReference         m_OutputStream;
};

/*----------------------------------------------------------------------
|   NPT_TlsClientSession
+---------------------------------------------------------------------*/
class NPT_TlsClientSession : public NPT_TlsSession
{
public:
    NPT_TlsClientSession(NPT_TlsContextReference&   context,
                         NPT_InputStreamReference&  input,
                         NPT_OutputStreamReference& output);
};

/*----------------------------------------------------------------------
|   NPT_TlsServerSession
+---------------------------------------------------------------------*/
class NPT_TlsServerSession : public NPT_TlsSession
{
public:
    NPT_TlsServerSession(NPT_TlsContextReference&   context,
                         NPT_InputStreamReference&  input,
                         NPT_OutputStreamReference& output);
};

/*----------------------------------------------------------------------
|   Trust Anchors
+-----------------------------------------------------------------*/
/** 
 * Array of trust anchors (each array element is of type NPT_TlsTrustAnchorData
 * and the last element is a terminator element: the cert_data field is NULL
 * and the cert_size field is 0
 */ 
extern const NPT_TlsTrustAnchorData NptTlsDefaultTrustAnchors[];

/* Verisign/RSA Secure Server CA */
const unsigned int  NptTlsTrustAnchor_0000_Size = 568;
extern const unsigned char NptTlsTrustAnchor_0000_Data[];

/* GTE CyberTrust Root CA */
const unsigned int  NptTlsTrustAnchor_0001_Size = 510;
extern const unsigned char NptTlsTrustAnchor_0001_Data[];

/* GTE CyberTrust Global Root */
const unsigned int  NptTlsTrustAnchor_0002_Size = 606;
extern const unsigned char NptTlsTrustAnchor_0002_Data[];

/* Thawte Personal Basic CA */
const unsigned int  NptTlsTrustAnchor_0003_Size = 805;
extern const unsigned char NptTlsTrustAnchor_0003_Data[];

/* Thawte Personal Premium CA */
const unsigned int  NptTlsTrustAnchor_0004_Size = 813;
extern const unsigned char NptTlsTrustAnchor_0004_Data[];

/* Thawte Personal Freemail CA */
const unsigned int  NptTlsTrustAnchor_0005_Size = 817;
extern const unsigned char NptTlsTrustAnchor_0005_Data[];

/* Thawte Server CA */
const unsigned int  NptTlsTrustAnchor_0006_Size = 791;
extern const unsigned char NptTlsTrustAnchor_0006_Data[];

/* Thawte Premium Server CA */
const unsigned int  NptTlsTrustAnchor_0007_Size = 811;
extern const unsigned char NptTlsTrustAnchor_0007_Data[];

/* Equifax Secure CA */
const unsigned int  NptTlsTrustAnchor_0008_Size = 804;
extern const unsigned char NptTlsTrustAnchor_0008_Data[];

/* ABAecom (sub., Am. Bankers Assn.) Root CA */
const unsigned int  NptTlsTrustAnchor_0009_Size = 953;
extern const unsigned char NptTlsTrustAnchor_0009_Data[];

/* Digital Signature Trust Co. Global CA 1 */
const unsigned int  NptTlsTrustAnchor_0010_Size = 813;
extern const unsigned char NptTlsTrustAnchor_0010_Data[];

/* Digital Signature Trust Co. Global CA 3 */
const unsigned int  NptTlsTrustAnchor_0011_Size = 813;
extern const unsigned char NptTlsTrustAnchor_0011_Data[];

/* Digital Signature Trust Co. Global CA 2 */
const unsigned int  NptTlsTrustAnchor_0012_Size = 988;
extern const unsigned char NptTlsTrustAnchor_0012_Data[];

/* Digital Signature Trust Co. Global CA 4 */
const unsigned int  NptTlsTrustAnchor_0013_Size = 988;
extern const unsigned char NptTlsTrustAnchor_0013_Data[];

/* Verisign Class 1 Public Primary Certification Authority */
const unsigned int  NptTlsTrustAnchor_0014_Size = 577;
extern const unsigned char NptTlsTrustAnchor_0014_Data[];

/* Verisign Class 2 Public Primary Certification Authority */
const unsigned int  NptTlsTrustAnchor_0015_Size = 576;
extern const unsigned char NptTlsTrustAnchor_0015_Data[];

/* Verisign Class 3 Public Primary Certification Authority */
const unsigned int  NptTlsTrustAnchor_0016_Size = 576;
extern const unsigned char NptTlsTrustAnchor_0016_Data[];

/* Verisign Class 1 Public Primary Certification Authority - G2 */
const unsigned int  NptTlsTrustAnchor_0017_Size = 774;
extern const unsigned char NptTlsTrustAnchor_0017_Data[];

/* Verisign Class 2 Public Primary Certification Authority - G2 */
const unsigned int  NptTlsTrustAnchor_0018_Size = 775;
extern const unsigned char NptTlsTrustAnchor_0018_Data[];

/* Verisign Class 3 Public Primary Certification Authority - G2 */
const unsigned int  NptTlsTrustAnchor_0019_Size = 774;
extern const unsigned char NptTlsTrustAnchor_0019_Data[];

/* Verisign Class 4 Public Primary Certification Authority - G2 */
const unsigned int  NptTlsTrustAnchor_0020_Size = 774;
extern const unsigned char NptTlsTrustAnchor_0020_Data[];

/* GlobalSign Root CA */
const unsigned int  NptTlsTrustAnchor_0021_Size = 889;
extern const unsigned char NptTlsTrustAnchor_0021_Data[];

/* GlobalSign Root CA - R2 */
const unsigned int  NptTlsTrustAnchor_0022_Size = 958;
extern const unsigned char NptTlsTrustAnchor_0022_Data[];

/* ValiCert Class 1 VA */
const unsigned int  NptTlsTrustAnchor_0023_Size = 747;
extern const unsigned char NptTlsTrustAnchor_0023_Data[];

/* ValiCert Class 2 VA */
const unsigned int  NptTlsTrustAnchor_0024_Size = 747;
extern const unsigned char NptTlsTrustAnchor_0024_Data[];

/* RSA Root Certificate 1 */
const unsigned int  NptTlsTrustAnchor_0025_Size = 747;
extern const unsigned char NptTlsTrustAnchor_0025_Data[];

/* Verisign Class 1 Public Primary Certification Authority - G3 */
const unsigned int  NptTlsTrustAnchor_0026_Size = 1054;
extern const unsigned char NptTlsTrustAnchor_0026_Data[];

/* Verisign Class 2 Public Primary Certification Authority - G3 */
const unsigned int  NptTlsTrustAnchor_0027_Size = 1053;
extern const unsigned char NptTlsTrustAnchor_0027_Data[];

/* Verisign Class 3 Public Primary Certification Authority - G3 */
const unsigned int  NptTlsTrustAnchor_0028_Size = 1054;
extern const unsigned char NptTlsTrustAnchor_0028_Data[];

/* Verisign Class 4 Public Primary Certification Authority - G3 */
const unsigned int  NptTlsTrustAnchor_0029_Size = 1054;
extern const unsigned char NptTlsTrustAnchor_0029_Data[];

/* Entrust.net Secure Server CA */
const unsigned int  NptTlsTrustAnchor_0030_Size = 1244;
extern const unsigned char NptTlsTrustAnchor_0030_Data[];

/* Entrust.net Secure Personal CA */
const unsigned int  NptTlsTrustAnchor_0031_Size = 1265;
extern const unsigned char NptTlsTrustAnchor_0031_Data[];

/* Entrust.net Premium 2048 Secure Server CA */
const unsigned int  NptTlsTrustAnchor_0032_Size = 1120;
extern const unsigned char NptTlsTrustAnchor_0032_Data[];

/* Baltimore CyberTrust Root */
const unsigned int  NptTlsTrustAnchor_0033_Size = 891;
extern const unsigned char NptTlsTrustAnchor_0033_Data[];

/* Equifax Secure Global eBusiness CA */
const unsigned int  NptTlsTrustAnchor_0034_Size = 660;
extern const unsigned char NptTlsTrustAnchor_0034_Data[];

/* Equifax Secure eBusiness CA 1 */
const unsigned int  NptTlsTrustAnchor_0035_Size = 646;
extern const unsigned char NptTlsTrustAnchor_0035_Data[];

/* Equifax Secure eBusiness CA 2 */
const unsigned int  NptTlsTrustAnchor_0036_Size = 804;
extern const unsigned char NptTlsTrustAnchor_0036_Data[];

/* Visa International Global Root 2 */
const unsigned int  NptTlsTrustAnchor_0037_Size = 900;
extern const unsigned char NptTlsTrustAnchor_0037_Data[];

/* beTRUSTed Root CA */
const unsigned int  NptTlsTrustAnchor_0038_Size = 1328;
extern const unsigned char NptTlsTrustAnchor_0038_Data[];

/* AddTrust Low-Value Services Root */
const unsigned int  NptTlsTrustAnchor_0039_Size = 1052;
extern const unsigned char NptTlsTrustAnchor_0039_Data[];

/* AddTrust External Root */
const unsigned int  NptTlsTrustAnchor_0040_Size = 1082;
extern const unsigned char NptTlsTrustAnchor_0040_Data[];

/* AddTrust Public Services Root */
const unsigned int  NptTlsTrustAnchor_0041_Size = 1049;
extern const unsigned char NptTlsTrustAnchor_0041_Data[];

/* AddTrust Qualified Certificates Root */
const unsigned int  NptTlsTrustAnchor_0042_Size = 1058;
extern const unsigned char NptTlsTrustAnchor_0042_Data[];

/* Verisign Time Stamping Authority CA */
const unsigned int  NptTlsTrustAnchor_0043_Size = 977;
extern const unsigned char NptTlsTrustAnchor_0043_Data[];

/* Thawte Time Stamping CA */
const unsigned int  NptTlsTrustAnchor_0044_Size = 677;
extern const unsigned char NptTlsTrustAnchor_0044_Data[];

/* Entrust.net Global Secure Server CA */
const unsigned int  NptTlsTrustAnchor_0045_Size = 1177;
extern const unsigned char NptTlsTrustAnchor_0045_Data[];

/* Entrust.net Global Secure Personal CA */
const unsigned int  NptTlsTrustAnchor_0046_Size = 1159;
extern const unsigned char NptTlsTrustAnchor_0046_Data[];

/* Entrust Root Certification Authority */
const unsigned int  NptTlsTrustAnchor_0047_Size = 1173;
extern const unsigned char NptTlsTrustAnchor_0047_Data[];

/* AOL Time Warner Root Certification Authority 1 */
const unsigned int  NptTlsTrustAnchor_0048_Size = 1002;
extern const unsigned char NptTlsTrustAnchor_0048_Data[];

/* AOL Time Warner Root Certification Authority 2 */
const unsigned int  NptTlsTrustAnchor_0049_Size = 1514;
extern const unsigned char NptTlsTrustAnchor_0049_Data[];

/* beTRUSTed Root CA-Baltimore Implementation */
const unsigned int  NptTlsTrustAnchor_0050_Size = 1390;
extern const unsigned char NptTlsTrustAnchor_0050_Data[];

/* beTRUSTed Root CA - Entrust Implementation */
const unsigned int  NptTlsTrustAnchor_0051_Size = 1621;
extern const unsigned char NptTlsTrustAnchor_0051_Data[];

/* beTRUSTed Root CA - RSA Implementation */
const unsigned int  NptTlsTrustAnchor_0052_Size = 1388;
extern const unsigned char NptTlsTrustAnchor_0052_Data[];

/* RSA Security 2048 v3 */
const unsigned int  NptTlsTrustAnchor_0053_Size = 869;
extern const unsigned char NptTlsTrustAnchor_0053_Data[];

/* RSA Security 1024 v3 */
const unsigned int  NptTlsTrustAnchor_0054_Size = 608;
extern const unsigned char NptTlsTrustAnchor_0054_Data[];

/* GeoTrust Global CA */
const unsigned int  NptTlsTrustAnchor_0055_Size = 856;
extern const unsigned char NptTlsTrustAnchor_0055_Data[];

/* GeoTrust Global CA 2 */
const unsigned int  NptTlsTrustAnchor_0056_Size = 874;
extern const unsigned char NptTlsTrustAnchor_0056_Data[];

/* GeoTrust Universal CA */
const unsigned int  NptTlsTrustAnchor_0057_Size = 1388;
extern const unsigned char NptTlsTrustAnchor_0057_Data[];

/* GeoTrust Universal CA 2 */
const unsigned int  NptTlsTrustAnchor_0058_Size = 1392;
extern const unsigned char NptTlsTrustAnchor_0058_Data[];

/* UTN-USER First-Network Applications */
const unsigned int  NptTlsTrustAnchor_0059_Size = 1128;
extern const unsigned char NptTlsTrustAnchor_0059_Data[];

/* America Online Root Certification Authority 1 */
const unsigned int  NptTlsTrustAnchor_0060_Size = 936;
extern const unsigned char NptTlsTrustAnchor_0060_Data[];

/* America Online Root Certification Authority 2 */
const unsigned int  NptTlsTrustAnchor_0061_Size = 1448;
extern const unsigned char NptTlsTrustAnchor_0061_Data[];

/* Visa eCommerce Root */
const unsigned int  NptTlsTrustAnchor_0062_Size = 934;
extern const unsigned char NptTlsTrustAnchor_0062_Data[];

/* TC TrustCenter, Germany, Class 2 CA */
const unsigned int  NptTlsTrustAnchor_0063_Size = 864;
extern const unsigned char NptTlsTrustAnchor_0063_Data[];

/* TC TrustCenter, Germany, Class 3 CA */
const unsigned int  NptTlsTrustAnchor_0064_Size = 864;
extern const unsigned char NptTlsTrustAnchor_0064_Data[];

/* Certum Root CA */
const unsigned int  NptTlsTrustAnchor_0065_Size = 784;
extern const unsigned char NptTlsTrustAnchor_0065_Data[];

/* Comodo AAA Services root */
const unsigned int  NptTlsTrustAnchor_0066_Size = 1078;
extern const unsigned char NptTlsTrustAnchor_0066_Data[];

/* Comodo Secure Services root */
const unsigned int  NptTlsTrustAnchor_0067_Size = 1091;
extern const unsigned char NptTlsTrustAnchor_0067_Data[];

/* Comodo Trusted Services root */
const unsigned int  NptTlsTrustAnchor_0068_Size = 1095;
extern const unsigned char NptTlsTrustAnchor_0068_Data[];

/* IPS Chained CAs root */
const unsigned int  NptTlsTrustAnchor_0069_Size = 2043;
extern const unsigned char NptTlsTrustAnchor_0069_Data[];

/* IPS CLASE1 root */
const unsigned int  NptTlsTrustAnchor_0070_Size = 2030;
extern const unsigned char NptTlsTrustAnchor_0070_Data[];

/* IPS CLASE3 root */
const unsigned int  NptTlsTrustAnchor_0071_Size = 2030;
extern const unsigned char NptTlsTrustAnchor_0071_Data[];

/* IPS CLASEA1 root */
const unsigned int  NptTlsTrustAnchor_0072_Size = 2043;
extern const unsigned char NptTlsTrustAnchor_0072_Data[];

/* IPS CLASEA3 root */
const unsigned int  NptTlsTrustAnchor_0073_Size = 2043;
extern const unsigned char NptTlsTrustAnchor_0073_Data[];

/* IPS Servidores root */
const unsigned int  NptTlsTrustAnchor_0074_Size = 699;
extern const unsigned char NptTlsTrustAnchor_0074_Data[];

/* IPS Timestamping root */
const unsigned int  NptTlsTrustAnchor_0075_Size = 2108;
extern const unsigned char NptTlsTrustAnchor_0075_Data[];

/* QuoVadis Root CA */
const unsigned int  NptTlsTrustAnchor_0076_Size = 1492;
extern const unsigned char NptTlsTrustAnchor_0076_Data[];

/* QuoVadis Root CA 2 */
const unsigned int  NptTlsTrustAnchor_0077_Size = 1467;
extern const unsigned char NptTlsTrustAnchor_0077_Data[];

/* QuoVadis Root CA 3 */
const unsigned int  NptTlsTrustAnchor_0078_Size = 1697;
extern const unsigned char NptTlsTrustAnchor_0078_Data[];

/* Security Communication Root CA */
const unsigned int  NptTlsTrustAnchor_0079_Size = 862;
extern const unsigned char NptTlsTrustAnchor_0079_Data[];

/* Sonera Class 1 Root CA */
const unsigned int  NptTlsTrustAnchor_0080_Size = 804;
extern const unsigned char NptTlsTrustAnchor_0080_Data[];

/* Sonera Class 2 Root CA */
const unsigned int  NptTlsTrustAnchor_0081_Size = 804;
extern const unsigned char NptTlsTrustAnchor_0081_Data[];

/* Staat der Nederlanden Root CA */
const unsigned int  NptTlsTrustAnchor_0082_Size = 958;
extern const unsigned char NptTlsTrustAnchor_0082_Data[];

/* TDC Internet Root CA */
const unsigned int  NptTlsTrustAnchor_0083_Size = 1071;
extern const unsigned char NptTlsTrustAnchor_0083_Data[];

/* TDC OCES Root CA */
const unsigned int  NptTlsTrustAnchor_0084_Size = 1309;
extern const unsigned char NptTlsTrustAnchor_0084_Data[];

/* UTN DATACorp SGC Root CA */
const unsigned int  NptTlsTrustAnchor_0085_Size = 1122;
extern const unsigned char NptTlsTrustAnchor_0085_Data[];

/* UTN USERFirst Email Root CA */
const unsigned int  NptTlsTrustAnchor_0086_Size = 1190;
extern const unsigned char NptTlsTrustAnchor_0086_Data[];

/* UTN USERFirst Hardware Root CA */
const unsigned int  NptTlsTrustAnchor_0087_Size = 1144;
extern const unsigned char NptTlsTrustAnchor_0087_Data[];

/* UTN USERFirst Object Root CA */
const unsigned int  NptTlsTrustAnchor_0088_Size = 1130;
extern const unsigned char NptTlsTrustAnchor_0088_Data[];

/* Camerfirma Chambers of Commerce Root */
const unsigned int  NptTlsTrustAnchor_0089_Size = 1217;
extern const unsigned char NptTlsTrustAnchor_0089_Data[];

/* Camerfirma Global Chambersign Root */
const unsigned int  NptTlsTrustAnchor_0090_Size = 1225;
extern const unsigned char NptTlsTrustAnchor_0090_Data[];

/* NetLock Qualified (Class QA) Root */
const unsigned int  NptTlsTrustAnchor_0091_Size = 1749;
extern const unsigned char NptTlsTrustAnchor_0091_Data[];

/* NetLock Notary (Class A) Root */
const unsigned int  NptTlsTrustAnchor_0092_Size = 1665;
extern const unsigned char NptTlsTrustAnchor_0092_Data[];

/* NetLock Business (Class B) Root */
const unsigned int  NptTlsTrustAnchor_0093_Size = 1359;
extern const unsigned char NptTlsTrustAnchor_0093_Data[];

/* NetLock Express (Class C) Root */
const unsigned int  NptTlsTrustAnchor_0094_Size = 1363;
extern const unsigned char NptTlsTrustAnchor_0094_Data[];

/* XRamp Global CA Root */
const unsigned int  NptTlsTrustAnchor_0095_Size = 1076;
extern const unsigned char NptTlsTrustAnchor_0095_Data[];

/* Go Daddy Class 2 CA */
const unsigned int  NptTlsTrustAnchor_0096_Size = 1028;
extern const unsigned char NptTlsTrustAnchor_0096_Data[];

/* Starfield Class 2 CA */
const unsigned int  NptTlsTrustAnchor_0097_Size = 1043;
extern const unsigned char NptTlsTrustAnchor_0097_Data[];

/* StartCom Ltd. */
const unsigned int  NptTlsTrustAnchor_0098_Size = 1306;
extern const unsigned char NptTlsTrustAnchor_0098_Data[];

/* StartCom Certification Authority */
const unsigned int  NptTlsTrustAnchor_0099_Size = 1997;
extern const unsigned char NptTlsTrustAnchor_0099_Data[];

/* Taiwan GRCA */
const unsigned int  NptTlsTrustAnchor_0100_Size = 1398;
extern const unsigned char NptTlsTrustAnchor_0100_Data[];

/* Firmaprofesional Root CA */
const unsigned int  NptTlsTrustAnchor_0101_Size = 1115;
extern const unsigned char NptTlsTrustAnchor_0101_Data[];

/* Wells Fargo Root CA */
const unsigned int  NptTlsTrustAnchor_0102_Size = 1001;
extern const unsigned char NptTlsTrustAnchor_0102_Data[];

/* Swisscom Root CA 1 */
const unsigned int  NptTlsTrustAnchor_0103_Size = 1501;
extern const unsigned char NptTlsTrustAnchor_0103_Data[];

/* DigiCert Assured ID Root CA */
const unsigned int  NptTlsTrustAnchor_0104_Size = 955;
extern const unsigned char NptTlsTrustAnchor_0104_Data[];

/* DigiCert Global Root CA */
const unsigned int  NptTlsTrustAnchor_0105_Size = 947;
extern const unsigned char NptTlsTrustAnchor_0105_Data[];

/* DigiCert High Assurance EV Root CA */
const unsigned int  NptTlsTrustAnchor_0106_Size = 969;
extern const unsigned char NptTlsTrustAnchor_0106_Data[];

/* Certplus Class 2 Primary CA */
const unsigned int  NptTlsTrustAnchor_0107_Size = 918;
extern const unsigned char NptTlsTrustAnchor_0107_Data[];

/* DST Root CA X3 */
const unsigned int  NptTlsTrustAnchor_0108_Size = 846;
extern const unsigned char NptTlsTrustAnchor_0108_Data[];

/* DST ACES CA X6 */
const unsigned int  NptTlsTrustAnchor_0109_Size = 1037;
extern const unsigned char NptTlsTrustAnchor_0109_Data[];

/* TURKTRUST Certificate Services Provider Root 1 */
const unsigned int  NptTlsTrustAnchor_0110_Size = 1023;
extern const unsigned char NptTlsTrustAnchor_0110_Data[];

/* TURKTRUST Certificate Services Provider Root 2 */
const unsigned int  NptTlsTrustAnchor_0111_Size = 1088;
extern const unsigned char NptTlsTrustAnchor_0111_Data[];

/* SwissSign Platinum CA - G2 */
const unsigned int  NptTlsTrustAnchor_0112_Size = 1477;
extern const unsigned char NptTlsTrustAnchor_0112_Data[];

/* SwissSign Gold CA - G2 */
const unsigned int  NptTlsTrustAnchor_0113_Size = 1470;
extern const unsigned char NptTlsTrustAnchor_0113_Data[];

/* SwissSign Silver CA - G2 */
const unsigned int  NptTlsTrustAnchor_0114_Size = 1473;
extern const unsigned char NptTlsTrustAnchor_0114_Data[];

/* GeoTrust Primary Certification Authority */
const unsigned int  NptTlsTrustAnchor_0115_Size = 896;
extern const unsigned char NptTlsTrustAnchor_0115_Data[];

/* thawte Primary Root CA */
const unsigned int  NptTlsTrustAnchor_0116_Size = 1060;
extern const unsigned char NptTlsTrustAnchor_0116_Data[];

/* VeriSign Class 3 Public Primary Certification Authority - G5 */
const unsigned int  NptTlsTrustAnchor_0117_Size = 1239;
extern const unsigned char NptTlsTrustAnchor_0117_Data[];

/* SecureTrust CA */
const unsigned int  NptTlsTrustAnchor_0118_Size = 956;
extern const unsigned char NptTlsTrustAnchor_0118_Data[];

/* Secure Global CA */
const unsigned int  NptTlsTrustAnchor_0119_Size = 960;
extern const unsigned char NptTlsTrustAnchor_0119_Data[];

/* COMODO Certification Authority */
const unsigned int  NptTlsTrustAnchor_0120_Size = 1057;
extern const unsigned char NptTlsTrustAnchor_0120_Data[];

/* DigiNotar Root CA */
const unsigned int  NptTlsTrustAnchor_0121_Size = 1422;
extern const unsigned char NptTlsTrustAnchor_0121_Data[];

/* Network Solutions Certificate Authority */
const unsigned int  NptTlsTrustAnchor_0122_Size = 1002;
extern const unsigned char NptTlsTrustAnchor_0122_Data[];

/* WellsSecure Public Root Certificate Authority */
const unsigned int  NptTlsTrustAnchor_0123_Size = 1217;
extern const unsigned char NptTlsTrustAnchor_0123_Data[];

/* COMODO ECC Certification Authority */
const unsigned int  NptTlsTrustAnchor_0124_Size = 1078;
extern const unsigned char NptTlsTrustAnchor_0124_Data[];

/* IGC/A */
const unsigned int  NptTlsTrustAnchor_0125_Size = 1030;
extern const unsigned char NptTlsTrustAnchor_0125_Data[];

/* Security Communication EV RootCA1 */
const unsigned int  NptTlsTrustAnchor_0126_Size = 897;
extern const unsigned char NptTlsTrustAnchor_0126_Data[];

/* OISTE WISeKey Global Root GA CA */
const unsigned int  NptTlsTrustAnchor_0127_Size = 1013;
extern const unsigned char NptTlsTrustAnchor_0127_Data[];

/* S-TRUST Authentication and Encryption Root CA 2005 PN */
const unsigned int  NptTlsTrustAnchor_0128_Size = 1151;
extern const unsigned char NptTlsTrustAnchor_0128_Data[];

/* Microsec e-Szigno Root CA */
const unsigned int  NptTlsTrustAnchor_0129_Size = 1964;
extern const unsigned char NptTlsTrustAnchor_0129_Data[];

/* Certigna */
const unsigned int  NptTlsTrustAnchor_0130_Size = 940;
extern const unsigned char NptTlsTrustAnchor_0130_Data[];

/* AC Ra\xC3\xADz Certic\xC3\xA1mara S.A. */
const unsigned int  NptTlsTrustAnchor_0131_Size = 1642;
extern const unsigned char NptTlsTrustAnchor_0131_Data[];

/* TC TrustCenter Class 2 CA II */
const unsigned int  NptTlsTrustAnchor_0132_Size = 1198;
extern const unsigned char NptTlsTrustAnchor_0132_Data[];

/* TC TrustCenter Class 3 CA II */
const unsigned int  NptTlsTrustAnchor_0133_Size = 1198;
extern const unsigned char NptTlsTrustAnchor_0133_Data[];

/* TC TrustCenter Universal CA I */
const unsigned int  NptTlsTrustAnchor_0134_Size = 993;
extern const unsigned char NptTlsTrustAnchor_0134_Data[];

/* Deutsche Telekom Root CA 2 */
const unsigned int  NptTlsTrustAnchor_0135_Size = 931;
extern const unsigned char NptTlsTrustAnchor_0135_Data[];

/* ComSign CA */
const unsigned int  NptTlsTrustAnchor_0136_Size = 919;
extern const unsigned char NptTlsTrustAnchor_0136_Data[];

/* ComSign Secured CA */
const unsigned int  NptTlsTrustAnchor_0137_Size = 943;
extern const unsigned char NptTlsTrustAnchor_0137_Data[];

/* Cybertrust Global Root */
const unsigned int  NptTlsTrustAnchor_0138_Size = 933;
extern const unsigned char NptTlsTrustAnchor_0138_Data[];

/* ePKI Root Certification Authority */
const unsigned int  NptTlsTrustAnchor_0139_Size = 1460;
extern const unsigned char NptTlsTrustAnchor_0139_Data[];

/* T\xc3\x9c\x42\xC4\xB0TAK UEKAE K\xC3\xB6k Sertifika Hizmet Sa\xC4\x9Flay\xc4\xb1\x63\xc4\xb1s\xc4\xb1 - S\xC3\xBCr\xC3\xBCm 3 */
const unsigned int  NptTlsTrustAnchor_0140_Size = 1307;
extern const unsigned char NptTlsTrustAnchor_0140_Data[];

/* Buypass Class 2 CA 1 */
const unsigned int  NptTlsTrustAnchor_0141_Size = 855;
extern const unsigned char NptTlsTrustAnchor_0141_Data[];

/* Buypass Class 3 CA 1 */
const unsigned int  NptTlsTrustAnchor_0142_Size = 855;
extern const unsigned char NptTlsTrustAnchor_0142_Data[];

/* EBG Elektronik Sertifika Hizmet Sa\xC4\x9Flay\xc4\xb1\x63\xc4\xb1s\xc4\xb1 */
const unsigned int  NptTlsTrustAnchor_0143_Size = 1515;
extern const unsigned char NptTlsTrustAnchor_0143_Data[];

/* certSIGN ROOT CA */
const unsigned int  NptTlsTrustAnchor_0144_Size = 828;
extern const unsigned char NptTlsTrustAnchor_0144_Data[];

/* CNNIC ROOT */
const unsigned int  NptTlsTrustAnchor_0145_Size = 857;
extern const unsigned char NptTlsTrustAnchor_0145_Data[];

/* ApplicationCA - Japanese Government */
const unsigned int  NptTlsTrustAnchor_0146_Size = 932;
extern const unsigned char NptTlsTrustAnchor_0146_Data[];

/* GeoTrust Primary Certification Authority - G3 */
const unsigned int  NptTlsTrustAnchor_0147_Size = 1026;
extern const unsigned char NptTlsTrustAnchor_0147_Data[];

/* thawte Primary Root CA - G2 */
const unsigned int  NptTlsTrustAnchor_0148_Size = 1070;
extern const unsigned char NptTlsTrustAnchor_0148_Data[];

/* GeoTrust Primary Certification Authority - G2 */
const unsigned int  NptTlsTrustAnchor_0149_Size = 1213;
extern const unsigned char NptTlsTrustAnchor_0149_Data[];

/* total anchors size = 164444 */

#endif // _NPT_TLS_H_
