/*****************************************************************
|
|   Neptune - Zip Support
|
|   (c) 2007 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

#ifndef _NPT_ZIP_H_
#define _NPT_ZIP_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "NptConfig.h"
#include "NptStreams.h"

/*----------------------------------------------------------------------
|   NPT_ZipInflatingInputStream
+---------------------------------------------------------------------*/
//class NPT_ZipInflatingInputStream : public NPT_InputStream 
//{
//public:
//    NPT_ZipInflatingInputStream(NPT_InputStreamReference& source);
//        
//private:
//    
//};

/*----------------------------------------------------------------------
|   NPT_ZipInflatingOutputStream
+---------------------------------------------------------------------*/

/*----------------------------------------------------------------------
|   NPT_ZipDeflatingInputStream
+---------------------------------------------------------------------*/

/*----------------------------------------------------------------------
|   NPT_ZipDeflatingInputStream
+---------------------------------------------------------------------*/

/*----------------------------------------------------------------------
|   NPT_Zip
+---------------------------------------------------------------------*/
const int NPT_ZIP_COMPRESSION_LEVEL_DEFAULT = -1;
const int NPT_ZIP_COMPRESSION_LEVEL_MIN     = 0;
const int NPT_ZIP_COMPRESSION_LEVEL_MAX     = 9;
const int NPT_ZIP_COMPRESSION_LEVEL_NONE    = 0;
class NPT_Zip 
{
public:
    /** 
     * Compressed data format
     */
    typedef enum {
        ZLIB,
        GZIP
    } Format;
        
    /**
     * Deflate (i.e compress) a buffer
     */
    static NPT_Result Deflate(const NPT_DataBuffer& in,
                              NPT_DataBuffer&       out,
                              int                   compression_level = NPT_ZIP_COMPRESSION_LEVEL_DEFAULT,
                              Format                format = ZLIB);
                              
    /**
     * Inflate (i.e decompress) a buffer
     */
    static NPT_Result Inflate(const NPT_DataBuffer& in,
                              NPT_DataBuffer&       out);
                       
private:
    static NPT_Result MapError(int err);
};

#endif // _NPT_ZIP_H_
