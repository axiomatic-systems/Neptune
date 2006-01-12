/*****************************************************************
|
|      Neptune - Ring Buffer
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

#ifndef _NPT_RING_BUFFER_H_
#define _NPT_RING_BUFFER_H_

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "NptTypes.h"

/*----------------------------------------------------------------------
|       NPT_RingBuffer
+---------------------------------------------------------------------*/
class NPT_RingBuffer
{
 public:
    // methods
                  NPT_RingBuffer(NPT_Size size);
    virtual      ~NPT_RingBuffer();
    NPT_Size      GetSpace() const;
    NPT_Size      GetContiguousSpace() const;
    NPT_Result    Write(const void* buffer, NPT_Size byte_count);
    NPT_Size      GetAvailable() const;
    NPT_Size      GetContiguousAvailable() const;
    NPT_Result    Read(void* buffer, NPT_Size byte_count);
    unsigned char ReadByte();
    unsigned char PeekByte(NPT_Offset offset);
    NPT_Result    MoveIn(NPT_Offset offset);
    NPT_Result    MoveOut(NPT_Offset offset);
    NPT_Result    Flush();

    // accessors
    unsigned char* GetReadPointer()  { return m_In; }
    unsigned char* GetWritePointer() { return m_Out;}

 private:
    // members
    struct {
        unsigned char* start;
        unsigned char* end;
    }              m_Data;
    unsigned char* m_In;
    unsigned char* m_Out;
    NPT_Size       m_Size;
};

#endif // _NPT_RING_BUFFER_H_
