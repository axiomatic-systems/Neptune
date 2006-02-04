/*****************************************************************
|
|      Neptune - Data Buffer
|
|      (c) 2001-2004 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "NptDataBuffer.h"
#include "NptUtils.h"
#include "NptResults.h"

/*----------------------------------------------------------------------
|       NPT_DataBuffer::NPT_DataBuffer
+---------------------------------------------------------------------*/
NPT_DataBuffer::NPT_DataBuffer() :
    m_BufferIsLocal(true),
    m_Buffer(NULL),
    m_BufferSize(0),
    m_DataSize(0)
{
}

/*----------------------------------------------------------------------
|       NPT_DataBuffer::NPT_DataBuffer
+---------------------------------------------------------------------*/
NPT_DataBuffer::NPT_DataBuffer(NPT_Size bufferSize) :
    m_BufferIsLocal(true),
    m_Buffer(new NPT_Byte[bufferSize]),
    m_BufferSize(bufferSize),
    m_DataSize(0)
{
}

/*----------------------------------------------------------------------
|       NPT_DataBuffer::NPT_DataBuffer
+---------------------------------------------------------------------*/
NPT_DataBuffer::NPT_DataBuffer(const void* data, NPT_Size dataSize) :
    m_BufferIsLocal(true),
    m_Buffer(new NPT_Byte[dataSize]),
    m_BufferSize(dataSize),
    m_DataSize(dataSize)
{
    NPT_CopyMemory(m_Buffer, data, dataSize);
}

/*----------------------------------------------------------------------
|       NPT_DataBuffer::NPT_DataBuffer
+---------------------------------------------------------------------*/
NPT_DataBuffer::NPT_DataBuffer(void* data, NPT_Size dataSize, bool copy) :
    m_BufferIsLocal(copy),
    m_Buffer(copy?new NPT_Byte[dataSize]:(NPT_Byte*)data),
    m_BufferSize(dataSize),
    m_DataSize(dataSize)
{
    if (copy) NPT_CopyMemory(m_Buffer, data, dataSize);
}

/*----------------------------------------------------------------------
|       NPT_DataBuffer::NPT_DataBuffer
+---------------------------------------------------------------------*/
NPT_DataBuffer::NPT_DataBuffer(const NPT_DataBuffer& other) :
    m_BufferIsLocal(true),
    m_Buffer(NULL),
    m_BufferSize(other.m_DataSize),
    m_DataSize(other.m_DataSize)
{
    m_Buffer = new NPT_Byte[m_BufferSize];
    NPT_CopyMemory(m_Buffer, other.m_Buffer, m_BufferSize);
}

/*----------------------------------------------------------------------
|       NPT_DataBuffer::~NPT_DataBuffer
+---------------------------------------------------------------------*/
NPT_DataBuffer::~NPT_DataBuffer()
{
    if (m_BufferIsLocal) {
        delete[] m_Buffer;
    }
}

/*----------------------------------------------------------------------
|       NPT_DataBuffer::SetBuffer
+---------------------------------------------------------------------*/
NPT_Result
NPT_DataBuffer::SetBuffer(NPT_Byte* buffer, NPT_Size buffer_size)
{
    if (m_BufferIsLocal) {
        // destroy the local buffer
        delete[] m_Buffer;
    }

    // we're now using an external buffer
    m_BufferIsLocal = false;
    m_Buffer = buffer;
    m_BufferSize = buffer_size;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_DataBuffer::SetBufferSize
+---------------------------------------------------------------------*/
NPT_Result
NPT_DataBuffer::SetBufferSize(NPT_Size buffer_size)
{
    if (m_BufferIsLocal) {
        return ReallocateBuffer(buffer_size);
    } else {
        return NPT_ERROR_NOT_SUPPORTED; // you cannot change the
                                        // buffer management mode
    }
}

/*----------------------------------------------------------------------
|       NPT_DataBuffer::SetDataSize
+---------------------------------------------------------------------*/
NPT_Result
NPT_DataBuffer::SetDataSize(NPT_Size size)
{
    if (size > m_BufferSize) {
        // the buffer is too small, we need to reallocate it
        if (m_BufferIsLocal) {
            NPT_CHECK(ReallocateBuffer(size));
        } else { 
            // we cannot reallocate an external buffer
            return NPT_ERROR_NOT_SUPPORTED;
        }
    }
    m_DataSize = size;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_DataBuffer::SetData
+---------------------------------------------------------------------*/
NPT_Result
NPT_DataBuffer::SetData(NPT_Byte* data, NPT_Size size)
{
    if (size > m_BufferSize) {
        if (m_BufferIsLocal) {
            NPT_CHECK(ReallocateBuffer(size));
        } else {
            return NPT_FAILURE;
        }
    }
    NPT_CopyMemory(m_Buffer, data, size);
    m_DataSize = size;

    return NPT_SUCCESS;
}


/*----------------------------------------------------------------------
|       NPT_DataBuffer::ReallocateBuffer
+---------------------------------------------------------------------*/
NPT_Result
NPT_DataBuffer::ReallocateBuffer(NPT_Size size)
{
    // check that the existing data fits
    if (m_DataSize > size) return NPT_FAILURE;

    // allocate a new buffer
    NPT_Byte* newBuffer = new NPT_Byte[size];

    // copy the contents of the previous buffer ,is any
	if (m_Buffer && m_DataSize) {
            NPT_CopyMemory(newBuffer, m_Buffer, m_DataSize);
	}

    // destroy the previous buffer
    delete[] m_Buffer;

    // use the new buffer
    m_Buffer = newBuffer;
    m_BufferSize = size;

    return NPT_SUCCESS;
}
