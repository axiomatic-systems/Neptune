/*****************************************************************
|
|   Neptune - Byte Streams
|
|   (c) 2001-2003 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

#ifndef _NPT_STREAMS_H_
#define _NPT_STREAMS_H_

/*----------------------------------------------------------------------
|    includes
+---------------------------------------------------------------------*/
#include "NptTypes.h"
#include "NptReferences.h"
#include "NptConstants.h"
#include "NptResults.h"
#include "NptDataBuffer.h"

/*----------------------------------------------------------------------
|    class references
+---------------------------------------------------------------------*/
class NPT_String;

/*----------------------------------------------------------------------
|    constants
+---------------------------------------------------------------------*/
const int NPT_ERROR_READ_FAILED  = NPT_ERROR_BASE_IO - 0;
const int NPT_ERROR_WRITE_FAILED = NPT_ERROR_BASE_IO - 1;
const int NPT_ERROR_EOS          = NPT_ERROR_BASE_IO - 2;

/*----------------------------------------------------------------------
|    NPT_InputStream
+---------------------------------------------------------------------*/
class NPT_InputStream
{
 public:
    // constructor and destructor
    virtual ~NPT_InputStream() {};

    // methods
    virtual NPT_Result Read(void*     buffer, 
                            NPT_Size  bytes_to_read, 
                            NPT_Size* bytes_read = NULL) = 0;
    virtual NPT_Result ReadFully(void*     buffer, 
                                 NPT_Size  bytes_to_read);
    virtual NPT_Result Seek(NPT_Offset offset) = 0;
    virtual NPT_Result Skip(NPT_Offset offset);
    virtual NPT_Result Tell(NPT_Offset& offset) = 0;
    virtual NPT_Result GetSize(NPT_Size& size) = 0;
    virtual NPT_Result GetAvailable(NPT_Size& available) = 0;
};

typedef NPT_Reference<NPT_InputStream> NPT_InputStreamReference;

/*----------------------------------------------------------------------
|    NPT_OutputStream
+---------------------------------------------------------------------*/
class NPT_OutputStream
{
public:
    // constructor and destructor
    virtual ~NPT_OutputStream() {};

    // methods
    virtual NPT_Result Write(const void* buffer, 
                             NPT_Size    bytes_to_write, 
                             NPT_Size*   bytes_written = NULL) = 0;
    virtual NPT_Result WriteFully(const void* buffer, 
                                  NPT_Size    bytes_to_write);
    virtual NPT_Result WriteString(const char* string_buffer);
    virtual NPT_Result WriteLine(const char* line_buffer);
    virtual NPT_Result Seek(NPT_Offset offset) = 0;
    virtual NPT_Result Tell(NPT_Offset& offset) = 0;
    virtual NPT_Result Flush() { return NPT_SUCCESS; }
};

typedef NPT_Reference<NPT_OutputStream> NPT_OutputStreamReference;

/*----------------------------------------------------------------------
|    NPT_StreamToStreamCopy
+---------------------------------------------------------------------*/
NPT_Result NPT_StreamToStreamCopy(NPT_InputStream*  from, 
                                  NPT_OutputStream* to,
                                  NPT_Offset        offset = 0,
                                  NPT_Size          size   = 0 /* 0 means the entire stream */);

/*----------------------------------------------------------------------
|    NPT_DelegatingInputStream
|
|    Use this class as a base class if you need to inherit both from
|    NPT_InputStream and NPT_OutputStream which share the Seek and Tell
|    method. In this case, you override the  base-specific version of 
|    those methods, InputSeek, InputTell, instead of the Seek and Tell 
|    methods.
+---------------------------------------------------------------------*/
class NPT_DelegatingInputStream : public NPT_InputStream
{
public:
    // NPT_InputStream methods
    NPT_Result Seek(NPT_Offset offset) {
        return InputSeek(offset);
    }
    NPT_Result Tell(NPT_Offset& offset) {
        return InputTell(offset);
    }

private:
    // methods
    virtual NPT_Result InputSeek(NPT_Offset  offset) = 0;
    virtual NPT_Result InputTell(NPT_Offset& offset) = 0;
};

/*----------------------------------------------------------------------
|    NPT_DelegatingOutputStream
|
|    Use this class as a base class if you need to inherit both from
|    NPT_InputStream and NPT_OutputStream which share the Seek and Tell
|    method. In this case, you override the  base-specific version of 
|    those methods, OutputSeek and OutputTell, instead of the Seek and 
|    Tell methods.
+---------------------------------------------------------------------*/
class NPT_DelegatingOutputStream : public NPT_OutputStream
{
public:
    // NPT_OutputStream methods
    NPT_Result Seek(NPT_Offset offset) {
        return OutputSeek(offset);
    }
    NPT_Result Tell(NPT_Offset& offset) {
        return OutputTell(offset);
    }

private:
    // methods
    virtual NPT_Result OutputSeek(NPT_Offset  offset) = 0;
    virtual NPT_Result OutputTell(NPT_Offset& offset) = 0;
};

/*----------------------------------------------------------------------
|    NPT_MemoryStream
+---------------------------------------------------------------------*/
class NPT_MemoryStream : 
    public NPT_DelegatingInputStream,
    public NPT_DelegatingOutputStream
{
public:
    // constructor and destructor
    NPT_MemoryStream();
    NPT_MemoryStream(void* data, NPT_Size size);
    virtual ~NPT_MemoryStream() {}

    // NPT_InputStream methods
    NPT_Result Read(void*     buffer, 
                    NPT_Size  bytes_to_read, 
                    NPT_Size* bytes_read = NULL);
    NPT_Result GetSize(NPT_Size& size)  { 
        size = m_Buffer.GetDataSize();    
        return NPT_SUCCESS;
    }
    NPT_Result GetAvailable(NPT_Size& available) { 
        available = m_Buffer.GetDataSize()-m_ReadOffset; 
        return NPT_SUCCESS;
    }

    // NPT_OutputStream methods
    NPT_Result Write(const void* buffer, 
                     NPT_Size    bytes_to_write, 
                     NPT_Size*   bytes_written = NULL);

    // methods delegated to m_Buffer
    const NPT_Byte* GetData() const { return m_Buffer.GetData(); }
    NPT_Byte*       UseData()       { return m_Buffer.UseData(); }

    // methods
    NPT_Result SetSize(NPT_Size size);

private:
    // NPT_DelegatingInputStream methods
    NPT_Result InputSeek(NPT_Offset offset);
    NPT_Result InputTell(NPT_Offset& offset) { 
        offset = m_ReadOffset; 
        return NPT_SUCCESS;
    }

    // NPT_DelegatingOutputStream methods
    NPT_Result OutputSeek(NPT_Offset offset);
    NPT_Result OutputTell(NPT_Offset& offset) {
        offset = m_WriteOffset; 
        return NPT_SUCCESS;
    }

protected:
    // members
    NPT_DataBuffer m_Buffer;
    NPT_Offset     m_ReadOffset;
    NPT_Offset     m_WriteOffset;
};

typedef NPT_Reference<NPT_MemoryStream> NPT_MemoryStreamReference;

#endif // _NPT_STREAMS_H_