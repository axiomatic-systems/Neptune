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
|       NPT_InputStream::ReadFully
+---------------------------------------------------------------------*/
NPT_Result
NPT_InputStream::ReadFully(void* buffer, NPT_Size bytes_to_read)
{
    // shortcut
    if (bytes_to_read == 0) return NPT_SUCCESS;

    // read until failure
    NPT_Size bytes_read;
    while (bytes_to_read) {
        NPT_Result result = Read(buffer, bytes_to_read, &bytes_read);
        if (NPT_FAILED(result)) return result;
        if (bytes_read == 0) return NPT_FAILURE;
        NPT_ASSERT(bytes_read <= bytes_to_read);
        bytes_to_read -= bytes_read;
        buffer = (void*)(((NPT_Byte*)buffer)+bytes_read);
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_InputStream::Skip
+---------------------------------------------------------------------*/
NPT_Result
NPT_InputStream::Skip(NPT_Size count)
{
    // get the current location
    NPT_Offset position;
    NPT_CHECK(Tell(position));

    // seek ahead
    return Seek(position+count);
}

/*----------------------------------------------------------------------
|       NPT_OutputStream::WriteFully
+---------------------------------------------------------------------*/
NPT_Result
NPT_OutputStream::WriteFully(const void* buffer, NPT_Size bytes_to_write)
{
    // shortcut
    if (bytes_to_write == 0) return NPT_SUCCESS;

    // write until failure
    NPT_Size bytes_written;
    while (bytes_to_write) {
        NPT_Result result = Write(buffer, bytes_to_write, &bytes_written);
        if (NPT_FAILED(result)) return result;
        if (bytes_written == 0) return NPT_FAILURE;
        NPT_ASSERT(bytes_written <= bytes_to_write);
        bytes_to_write -= bytes_written;
        buffer = (const void*)(((const NPT_Byte*)buffer)+bytes_written);
    }

    return NPT_SUCCESS;
}
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
    return WriteFully((const void*)buffer, string_length);
}

/*----------------------------------------------------------------------
|       NPT_OutputStream::WriteLine
+---------------------------------------------------------------------*/
NPT_Result
NPT_OutputStream::WriteLine(const char* buffer)
{
    NPT_CHECK(WriteString(buffer));
    NPT_CHECK(WriteFully((const void*)"\r\n", 2));

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
    if (bytes_to_read == 0) {
        if (bytes_read) *bytes_read = 0;
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
        NPT_CHECK(m_Buffer.SetBufferSize(allocate));
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
    NPT_CHECK(m_Buffer.SetDataSize(size));

    // adjust the read and write offsets
    if (m_ReadOffset > size) m_ReadOffset = size;
    if (m_WriteOffset > size) m_WriteOffset = size;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_MemoryStream::SetSize
+---------------------------------------------------------------------*/
const unsigned int NPT_STREAM_COPY_BUFFER_SIZE = 4096; // copy 4k at a time
NPT_Result 
NPT_StreamToStreamCopy(NPT_InputStream*  from, 
                       NPT_OutputStream* to,
                       NPT_Offset        offset /* = 0 */,
                       NPT_Size          size   /* = 0, 0 means the entire stream */)
{
    // check parameters
    if (from == NULL || to == NULL) return NPT_ERROR_INVALID_PARAMETERS;

    // seek into the input if required
    if (offset) {
        NPT_CHECK(from->Seek(offset));
    }

    // allocate a buffer for the transfer
    NPT_Size bytes_transfered = 0;
    NPT_Byte* buffer = new NPT_Byte[NPT_STREAM_COPY_BUFFER_SIZE];
    NPT_Result result = NPT_SUCCESS;
    if (buffer == NULL) return NPT_ERROR_OUT_OF_MEMORY;

    // copy until an error occurs or the end of stream is reached
    for (;;) {
        // read some data
        NPT_Size   bytes_to_read = NPT_STREAM_COPY_BUFFER_SIZE;
        NPT_Size   bytes_read = 0;
        if (size) {
            // a max size was specified
            if (bytes_to_read > size-bytes_transfered) {
                bytes_to_read = size-bytes_transfered;
            }
        }
        result = from->Read(buffer, bytes_to_read, &bytes_read);
        if (NPT_FAILED(result)) {
            if (result == NPT_ERROR_EOS) result = NPT_SUCCESS;
            break;
        }
        if (bytes_read == 0) continue;
        
        // write the data
        result = to->WriteFully(buffer, bytes_read);
        if (NPT_FAILED(result)) break;

        // update the counts
        if (size) {
            bytes_transfered += bytes_read;
            if (bytes_transfered >= size) break;
        }
    }

    // free the buffer and return
    delete[] buffer;
    return result;
}
