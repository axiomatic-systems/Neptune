/*****************************************************************
|
|      Neptune - Byte Streams
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "NptStreams.h"
#include "NptUtils.h"
#include "NptConstants.h"
#include "NptStrings.h"
#include "NptDebug.h"

/*----------------------------------------------------------------------
|       NPT_OutputStream::WriteString
+---------------------------------------------------------------------*/
NPT_Result
NPT_OutputStream::WriteString(const char* buffer)
{
    // shortcut
    NPT_Size string_length;
    if (buffer == NULL || (string_length = NPT_StringLength(buffer)) == 0) {
	    return NPT_SUCCESS;
    }

    // write the string
    return Write((const void*)buffer, string_length, NULL);
}

/*----------------------------------------------------------------------
|       NPT_OutputStream::WriteLine
+---------------------------------------------------------------------*/
NPT_Result
NPT_OutputStream::WriteLine(const char* buffer)
{
    NPT_CHECK(WriteString(buffer));
    NPT_CHECK(Write((const void*)"\r\n", 2, NULL));

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_MemoryStream::NPT_MemoryStream
+---------------------------------------------------------------------*/
NPT_MemoryStream::NPT_MemoryStream() : 
    m_ReadOffset(0),
    m_WriteOffset(0)
{
}

/*----------------------------------------------------------------------
|       NPT_MemoryStream::NPT_MemoryStream
+---------------------------------------------------------------------*/
NPT_MemoryStream::NPT_MemoryStream(void* data, NPT_Size size) : 
    m_Buffer(data, size),
    m_ReadOffset(0),
    m_WriteOffset(0)
{
}

/*----------------------------------------------------------------------
|       NPT_MemoryStream::Read
+---------------------------------------------------------------------*/
NPT_Result 
NPT_MemoryStream::Read(void*     buffer, 
                       NPT_Size  bytes_to_read, 
                       NPT_Size* bytes_read)
{
    // check for shortcut
    if (bytes_read == 0) {
        if (*bytes_read) *bytes_read = 0;
        return NPT_SUCCESS;
    }

    // clip to what's available
    NPT_Size available = m_Buffer.GetDataSize();
    if (m_ReadOffset+bytes_to_read > available) {
        bytes_to_read = available-m_ReadOffset;
    }

    // copy the data
    if (bytes_to_read) {
        NPT_CopyMemory(buffer, (void*)(((char*)m_Buffer.UseData())+m_ReadOffset), bytes_to_read);
        m_ReadOffset += bytes_to_read;
    } 
    if (bytes_read) *bytes_read = bytes_to_read;

    return bytes_to_read?NPT_SUCCESS:NPT_ERROR_EOS; 
}

/*----------------------------------------------------------------------
|       NPT_MemoryStream::InputSeek
+---------------------------------------------------------------------*/
NPT_Result 
NPT_MemoryStream::InputSeek(NPT_Offset offset)
{
    if (offset > m_Buffer.GetDataSize()) {
        return NPT_ERROR_INVALID_PARAMETERS;
    } else {
        m_ReadOffset = offset;
        return NPT_SUCCESS;
    }
}

/*----------------------------------------------------------------------
|       NPT_MemoryStream::Write
+---------------------------------------------------------------------*/
NPT_Result 
NPT_MemoryStream::Write(const void* data, 
                        NPT_Size    bytes_to_write, 
                        NPT_Size*   bytes_written)
{
    NPT_Size space_available = m_Buffer.GetBufferSize();
    NPT_Size space_needed = m_WriteOffset+bytes_to_write;
    if (space_needed > space_available) {
        NPT_Size allocate = space_available?
                            space_available*2: // try to double
                            1024;              // start with 1k
        if (allocate < space_needed) allocate = space_needed;
        NPT_Result result = m_Buffer.SetBufferSize(allocate);
        if (NPT_FAILED(result)) return result;
    }

    NPT_CopyMemory(m_Buffer.UseData()+m_WriteOffset, data, bytes_to_write);
    m_WriteOffset += bytes_to_write;
    if (m_WriteOffset > m_Buffer.GetDataSize()) {
        m_Buffer.SetDataSize(m_WriteOffset);
    }
    if (bytes_written) *bytes_written = bytes_to_write;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_MemoryStream::OutputSeek
+---------------------------------------------------------------------*/
NPT_Result 
NPT_MemoryStream::OutputSeek(NPT_Offset offset)
{
    if (offset <= m_Buffer.GetDataSize()) {
        m_WriteOffset = offset;
        return NPT_SUCCESS;
    } else {
        return NPT_ERROR_INVALID_PARAMETERS;
    }
}

/*----------------------------------------------------------------------
|       NPT_MemoryStream::SetSize
+---------------------------------------------------------------------*/
NPT_Result 
NPT_MemoryStream::SetSize(NPT_Size size)
{
    // try to resize the data buffer
    NPT_Result result = m_Buffer.SetDataSize(size);
    if (NPT_FAILED(result)) return result;

    // adjust the read and write offsets
    if (m_ReadOffset > size) m_ReadOffset = size;
    if (m_WriteOffset > size) m_WriteOffset = size;

    return NPT_SUCCESS;
}
