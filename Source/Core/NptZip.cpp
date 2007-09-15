/*****************************************************************
|
|   Neptune - Zip Support
|
|   (c) 2007 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "NptConfig.h"
#include "NptZip.h"
#include "NptLogging.h"
#include "NptUtils.h"

#include "zlib.h"

/*----------------------------------------------------------------------
|   logging
+---------------------------------------------------------------------*/
NPT_SET_LOCAL_LOGGER("neptune.zip")

/*----------------------------------------------------------------------
|   NPT_Zip::MapError
+---------------------------------------------------------------------*/
NPT_Result
NPT_Zip::MapError(int err)
{
    switch (err) {
        case Z_OK:            return NPT_SUCCESS;           
        case Z_STREAM_END:    return NPT_ERROR_EOS;
        case Z_DATA_ERROR:
        case Z_STREAM_ERROR:  return NPT_ERROR_INVALID_FORMAT;
        case Z_MEM_ERROR:     return NPT_ERROR_OUT_OF_MEMORY;
        case Z_VERSION_ERROR: return NPT_ERROR_INTERNAL;
        case Z_NEED_DICT:     return NPT_ERROR_NOT_SUPPORTED;
        default:              return NPT_FAILURE;
    }
}

/*----------------------------------------------------------------------
|   NPT_Zip::Deflate
+---------------------------------------------------------------------*/
NPT_Result 
NPT_Zip::Deflate(const NPT_DataBuffer& in,
                 NPT_DataBuffer&       out,
                 int                   compression_level,
                 Format                format /* = ZLIB */)
{
    // default return state
    out.SetDataSize(0);
    
    // check parameters
    if (compression_level < NPT_ZIP_COMPRESSION_LEVEL_DEFAULT ||
        compression_level > NPT_ZIP_COMPRESSION_LEVEL_MAX) {
        return NPT_ERROR_INVALID_PARAMETERS;
    }
                
    // setup the stream
    z_stream stream;
    NPT_SetMemory(&stream, 0, sizeof(stream));
    stream.next_in   = (Bytef*)in.GetData();
    stream.avail_in  = (uInt)in.GetDataSize();
    
    // setup the memory functions
    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    // initialize the compressor
    int err = deflateInit2(&stream, 
                           compression_level,
                           Z_DEFLATED,
                           15 + (format == GZIP ? 16 : 0),
                           8,
                           Z_DEFAULT_STRATEGY);
    if (err != Z_OK) return MapError(err);

    // reserve an output buffer known to be large enough
    out.Reserve(deflateBound(&stream, stream.avail_in) + (format==GZIP?10:0));
    stream.next_out  = out.UseData();
    stream.avail_out = out.GetBufferSize();

    // decompress
    err = deflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        deflateEnd(&stream);
        return MapError(err);
    }
    
    // update the output size
    out.SetDataSize(stream.total_out);

    // cleanup
    err = deflateEnd(&stream);
    return MapError(err);
}
                              
/*----------------------------------------------------------------------
|   NPT_Zip::Inflate
+---------------------------------------------------------------------*/                              
NPT_Result 
NPT_Zip::Inflate(const NPT_DataBuffer& in,
                 NPT_DataBuffer&       out)
{
    // assume an output buffer twice the size of the input plus a bit
    NPT_CHECK_WARNING(out.Reserve(32+2*in.GetDataSize()));
    
    // setup the stream
    z_stream stream;
    stream.next_in   = (Bytef*)in.GetData();
    stream.avail_in  = (uInt)in.GetDataSize();
    stream.next_out  = out.UseData();
    stream.avail_out = (uInt)out.GetBufferSize();

    // setup the memory functions
    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    // initialize the compressor
    int err = inflateInit2(&stream, 15+32); // 15 = default window bits, +32 = automatic header
    if (err != Z_OK) return MapError(err);
    
    // decompress until the end
    do {
        err = inflate(&stream, Z_SYNC_FLUSH);
        if (err == Z_STREAM_END || err == Z_OK || err == Z_BUF_ERROR) {
            out.SetDataSize(stream.total_out);
            if ((err == Z_OK && stream.avail_out == 0) || err == Z_BUF_ERROR) {
                // grow the output buffer
                out.Reserve(out.GetBufferSize()*2);
                stream.next_out = out.UseData()+stream.total_out;
                stream.avail_out = out.GetBufferSize()-stream.total_out;
            }
        }
    } while (err == Z_OK);
    
    // check for errors
    if (err != Z_STREAM_END) {
        inflateEnd(&stream);
        return MapError(err);
    }
    
    // cleanup
    err = inflateEnd(&stream);
    return MapError(err);
}
