/*****************************************************************
|
|   Neptune - Files
|
|   (c) 2001-2006 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

#ifndef _NPT_FILE_H_
#define _NPT_FILE_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "NptTypes.h"
#include "NptStreams.h"

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
const int NPT_ERROR_NO_SUCH_FILE          = NPT_ERROR_BASE_FILE - 0;
const int NPT_ERROR_FILE_NOT_OPEN         = NPT_ERROR_BASE_FILE - 1;
const int NPT_ERROR_FILE_BUSY             = NPT_ERROR_BASE_FILE - 2;
const int NPT_ERROR_FILE_ALREADY_OPEN     = NPT_ERROR_BASE_FILE - 3;
const int NPT_ERROR_FILE_NOT_READABLE     = NPT_ERROR_BASE_FILE - 4;
const int NPT_ERROR_FILE_NOT_WRITABLE     = NPT_ERROR_BASE_FILE - 5;
const int NPT_ERROR_FILE_NOT_DIRECTORY    = NPT_ERROR_BASE_FILE - 6;
const int NPT_ERROR_FILE_ALREADY_EXISTS   = NPT_ERROR_BASE_FILE - 7;
const int NPT_ERROR_FILE_NOT_ENOUGH_SPACE = NPT_ERROR_BASE_FILE - 8;
const int NPT_ERROR_DIRECTORY_NOT_EMPTY   = NPT_ERROR_BASE_FILE - 9;

const unsigned int NPT_FILE_OPEN_MODE_READ       = 0x01;
const unsigned int NPT_FILE_OPEN_MODE_WRITE      = 0x02;
const unsigned int NPT_FILE_OPEN_MODE_CREATE     = 0x04;
const unsigned int NPT_FILE_OPEN_MODE_TRUNCATE   = 0x08;
const unsigned int NPT_FILE_OPEN_MODE_UNBUFFERED = 0x10;
const unsigned int NPT_FILE_OPEN_MODE_APPEND     = 0x20;

const unsigned int NPT_FILE_ATTRIBUTE_READ_ONLY = 0x01;
const unsigned int NPT_FILE_ATTRIBUTE_LINK      = 0x02;

#define NPT_FILE_STANDARD_INPUT  "@STDIN"
#define NPT_FILE_STANDARD_OUTPUT "@STDOUT"
#define NPT_FILE_STANDARD_ERROR  "@STDERR"

/*----------------------------------------------------------------------
|   class references
+---------------------------------------------------------------------*/
class NPT_DataBuffer;

/*----------------------------------------------------------------------
|   NPT_FileInfo
+---------------------------------------------------------------------*/
struct NPT_FileInfo
{
    // types
    typedef enum {
        FILE_TYPE_NONE,
        FILE_TYPE_REGULAR,
        FILE_TYPE_DIRECTORY,
        FILE_TYPE_SPECIAL,
        FILE_TYPE_OTHER
    } FileType;
    
    // constructor
    NPT_FileInfo() : m_Type(FILE_TYPE_NONE), m_Size(0), m_AttributesMask(0), m_Attributes(0) {}
    
    // members
    FileType   m_Type;
    NPT_UInt64 m_Size;
    NPT_Flags  m_AttributesMask;
    NPT_Flags  m_Attributes;
};

/*----------------------------------------------------------------------
|   NPT_FilePath
+---------------------------------------------------------------------*/
class NPT_FilePath
{
public:
    // class members
    static const NPT_String Separator;

    // class methods
    static NPT_String BaseName(const char* path);
    static NPT_String DirectoryName(const char* path);
    static NPT_String FileExtension(const char* path);
    
private:
    NPT_FilePath() {} // this class can't have instances
};

/*----------------------------------------------------------------------
|   NPT_FileInterface
+---------------------------------------------------------------------*/
class NPT_FileInterface
{
public:
    // types
    typedef unsigned int OpenMode;

    // constructors and destructor
    virtual ~NPT_FileInterface() {}

    // methods
    virtual NPT_Result Open(OpenMode mode) = 0;
    virtual NPT_Result Close() = 0;
    virtual NPT_Result GetInputStream(NPT_InputStreamReference& stream) = 0;
    virtual NPT_Result GetOutputStream(NPT_OutputStreamReference& stream) = 0;
};

/*----------------------------------------------------------------------
|   NPT_File
+---------------------------------------------------------------------*/
class NPT_File : public NPT_FileInterface
{
public:
    // class methods
    static NPT_Result GetRoots(NPT_List<NPT_String>& roots);
    static NPT_Result GetInfo(const char* path, NPT_FileInfo* info = NULL);
    static bool       Exists(const char* path) { return NPT_SUCCEEDED(GetInfo(path)); }
    static NPT_Result DeleteFile(const char* path);
    static NPT_Result DeleteDirectory(const char* path);
    static NPT_Result Rename(const char* from_path, const char* to_path);
    static NPT_Result ListDirectory(const char* path, NPT_List<NPT_String>& entries);
    static NPT_Result CreateDirectory(const char* path);
    static NPT_Result GetWorkingDirectory(NPT_String& path);
    static NPT_Result Load(const char* path, NPT_DataBuffer& buffer);
    
    // constructors and destructor
    NPT_File(const char* path);
   ~NPT_File() { delete m_Delegate; }

    // methods
    NPT_Result          Load(NPT_DataBuffer& buffer);
    const NPT_String&   GetPath() { return m_Path; }
    NPT_Result          GetSize(NPT_Size& size);
    NPT_Result          GetSize(NPT_LargeSize &size);
    NPT_Result          GetInfo(NPT_FileInfo& info);
    NPT_Result          ListDirectory(NPT_List<NPT_String>& entries);
    NPT_Result          Rename(const char* path);
    
    // NPT_FileInterface methods
    NPT_Result Open(OpenMode mode) {
        return m_Delegate->Open(mode);
    }
    NPT_Result Close() {
        return m_Delegate->Close();
    }
    NPT_Result GetInputStream(NPT_InputStreamReference& stream) {
        return m_Delegate->GetInputStream(stream);
    }
    NPT_Result GetOutputStream(NPT_OutputStreamReference& stream) {
        return m_Delegate->GetOutputStream(stream);
    }

protected:
    // members
    NPT_FileInterface* m_Delegate;
    NPT_String         m_Path;
    NPT_FileInfo       m_Info;
};

#endif // _NPT_FILE_H_ 
