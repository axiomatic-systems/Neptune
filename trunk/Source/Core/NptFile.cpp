/*****************************************************************
|
|   Neptune - Files
|
|   (c) 2001-2006 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "NptFile.h"
#include "NptUtils.h"
#include "NptConstants.h"
#include "NptStreams.h"
#include "NptDataBuffer.h"
#include "NptLogging.h"

/*----------------------------------------------------------------------
|   logging
+---------------------------------------------------------------------*/
NPT_SET_LOCAL_LOGGER("neptune.file")

/*----------------------------------------------------------------------
|   NPT_FilePath::BaseName
+---------------------------------------------------------------------*/
NPT_String 
NPT_FilePath::BaseName(const char* path)
{
    NPT_String result = path;
    int separator = result.ReverseFind(Separator);
    if (separator >= 0) {
        result = path+separator+NPT_StringLength(Separator);
    } 

    return result;
}

/*----------------------------------------------------------------------
|   NPT_FilePath::DirectoryName
+---------------------------------------------------------------------*/
NPT_String 
NPT_FilePath::DirectoryName(const char* path)
{
    NPT_String result = path;
    int separator = result.ReverseFind(Separator);
    if (separator >= 0) {
        if (separator == 0) {
            result.SetLength(NPT_StringLength(Separator));
        } else {
            result.SetLength(separator);
        }
    } else {
        result.SetLength(0);
    } 

    return result;
}

/*----------------------------------------------------------------------
|   NPT_FilePath::FileExtension
+---------------------------------------------------------------------*/
NPT_String 
NPT_FilePath::FileExtension(const char* path)
{
    NPT_String result = path;
    int separator = result.ReverseFind('.');
    if (separator >= 0) {
        result = path+separator;
    } else {
        result.SetLength(0);
    }

    return result;
}

/*----------------------------------------------------------------------
|   NPT_File::Load
+---------------------------------------------------------------------*/
NPT_Result
NPT_File::Load(const char* path, NPT_DataBuffer& buffer)
{
    // create and open the file
    NPT_File file(path);
    NPT_Result result = file.Open(NPT_FILE_OPEN_MODE_READ);
    if (NPT_FAILED(result)) return result;
    
    // load the file
    result = file.Load(buffer);

    // close the file
    file.Close();

    return result;
}

/*----------------------------------------------------------------------
|   NPT_File::Load
+---------------------------------------------------------------------*/
NPT_Result
NPT_File::Load(NPT_DataBuffer& buffer)
{
    NPT_InputStreamReference input;

    // get the input stream for the file
    NPT_CHECK_WARNING(GetInputStream(input));

    // read the stream
    return input->Load(buffer);
}

/*----------------------------------------------------------------------
|   NPT_File::GetInfo
+---------------------------------------------------------------------*/
NPT_Result
NPT_File::GetInfo(NPT_FileInfo& info)
{
    NPT_Result result = NPT_SUCCESS;
    
    // get the file info if we don't already have it
    if (m_Info.m_Type == NPT_FileInfo::FILE_TYPE_NONE) {
        result = GetInfo(m_Path.GetChars(), &m_Info);
    }
    
    info = m_Info;
    
    return result;
}

/*----------------------------------------------------------------------
|   NPT_File::GetSize
+---------------------------------------------------------------------*/
NPT_Result
NPT_File::GetSize(NPT_LargeSize& size)
{
    // default value
    size = 0;
    
    // get the size from the info (call GetInfo() in case it has not
    // yet been called)
    NPT_FileInfo info;
    GetInfo(info);
    size = info.m_Size;
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_File::Rename
+---------------------------------------------------------------------*/
NPT_Result
NPT_File::Rename(const char* path)
{
    NPT_Result result = Rename(m_Path.GetChars(), path);
    if (NPT_SUCCEEDED(result)) {
        m_Path = path;
    }
    return result;
}

/*----------------------------------------------------------------------
|   NPT_File::ListDirectory
+---------------------------------------------------------------------*/
NPT_Result        
NPT_File::ListDirectory(NPT_List<NPT_String>& entries)
{
    entries.Clear();
    return ListDirectory(m_Path.GetChars(), entries);
}


