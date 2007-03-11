/*****************************************************************
|
|   Neptune - Files :: Standard C Implementation
|
|   (c) 2001-2006 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "f32file.h"

#include "NptUtils.h"
#include "NptFile.h"
#include "NptThreads.h"
#include "NptInterfaces.h"
#include "NptStrings.h"
#include "NptDebug.h"


/*----------------------------------------------------------------------
|   NPT_SymbOSFileWrapper
+---------------------------------------------------------------------*/
class NPT_SymbOSFileWrapper
{
public:
    // constructors and destructor
    NPT_SymbOSFileWrapper(RFs* fsSession, RFile* file) : m_FilesystemSession(fsSession), m_FileHandle(file) {}
    ~NPT_SymbOSFileWrapper() {
        if (m_FileHandle != NULL) {
            m_FileHandle->Close();
            delete m_FileHandle;
        }
        if (m_FilesystemSession != NULL) {
            m_FilesystemSession->Close();
            delete m_FilesystemSession;
        }
    }

    // methods
    RFile* GetFile() { return m_FileHandle; }

private:
    // members
    RFile*  m_FileHandle;
    RFs*    m_FilesystemSession;
};

typedef NPT_Reference<NPT_SymbOSFileWrapper> NPT_SymbOSFileReference;

/*----------------------------------------------------------------------
|   NPT_SymbOSFileStream
+---------------------------------------------------------------------*/
class NPT_SymbOSFileStream
{
public:
    // constructors and destructor
    NPT_SymbOSFileStream(NPT_SymbOSFileReference file) :
      m_FileReference(file) {}

    // NPT_FileInterface methods
    NPT_Result Seek(NPT_Position offset);
    NPT_Result Tell(NPT_Position& offset);

protected:
    // constructors and destructors
    virtual ~NPT_SymbOSFileStream() {}

    // members
    NPT_SymbOSFileReference m_FileReference;
};

/*----------------------------------------------------------------------
|   NPT_SymbOSFileStream::Seek
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbOSFileStream::Seek(NPT_Position offset)
{
    return (m_FileReference->GetFile()->Seek(ESeekStart, (NPT_Int32&) offset) == KErrNone) ? NPT_SUCCESS : NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   NPT_SymbOSFileStream::Tell
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbOSFileStream::Tell(NPT_Position& offset)
{
    return (m_FileReference->GetFile()->Seek(ESeekCurrent, (NPT_Int32&) offset) == KErrNone) ? NPT_SUCCESS : NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   NPT_SymbOSFileInputStream
+---------------------------------------------------------------------*/
class NPT_SymbOSFileInputStream : public NPT_InputStream,
                                  private NPT_SymbOSFileStream
                                
{
public:
    // constructors and destructor
    NPT_SymbOSFileInputStream(NPT_SymbOSFileReference& file, NPT_Size size) :
        NPT_SymbOSFileStream(file), m_Size(size) {}

    // NPT_InputStream methods
    NPT_Result Read(void*     buffer, 
                    NPT_Size  bytes_to_read, 
                    NPT_Size* bytes_read);
    NPT_Result Seek(NPT_Position offset) {
        return NPT_SymbOSFileStream::Seek(offset);
    }
    NPT_Result Tell(NPT_Position& offset) {
        return NPT_SymbOSFileStream::Tell(offset);
    }
    NPT_Result GetSize(NPT_Size& size);
    NPT_Result GetAvailable(NPT_Size& available);

private:
    // members
    NPT_Size m_Size;
};

/*----------------------------------------------------------------------
|   NPT_SymbOSFileInputStream::Read
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbOSFileInputStream::Read(void*     buffer, 
                                NPT_Size  bytes_to_read, 
                                NPT_Size* bytes_read)
{
    // check the parameters
    if (buffer == NULL) {
        return NPT_ERROR_INVALID_PARAMETERS;
    }
    
    RBuf8 buf;
    buf.Create(bytes_to_read);

    // read from the file
    if (m_FileReference->GetFile()->Read(buf, bytes_to_read) == KErrNone) {
        NPT_Size nb_read = buf.Length();
        if (bytes_read) *bytes_read = nb_read;
        if (nb_read == 0)
            return NPT_ERROR_EOS;
        else
            return NPT_SUCCESS;
    } else {
        if (bytes_read) *bytes_read = 0;
        return NPT_ERROR_READ_FAILED;
    }
}

/*----------------------------------------------------------------------
|   NPT_SymbOSFileInputStream::GetSize
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbOSFileInputStream::GetSize(NPT_Size& size)
{
    size = m_Size;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbOSFileInputStream::GetAvailable
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbOSFileInputStream::GetAvailable(NPT_Size& available)
{
    NPT_Position offset = 0;
    Tell(offset);
    if (offset >= 0 && (NPT_Size)offset <= m_Size) {
        available = m_Size - offset;
        return NPT_SUCCESS;
    } else {
        available = 0;
        return NPT_FAILURE;
    }
}

/*----------------------------------------------------------------------
|   NPT_SymbOSFileOutputStream
+---------------------------------------------------------------------*/
class NPT_SymbOSFileOutputStream : public NPT_OutputStream,
                                 private NPT_SymbOSFileStream
{
public:
    // constructors and destructor
    NPT_SymbOSFileOutputStream(NPT_SymbOSFileReference& file) :
        NPT_SymbOSFileStream(file) {}

    // NPT_InputStream methods
    NPT_Result Write(const void* buffer, 
                     NPT_Size    bytes_to_write, 
                     NPT_Size*   bytes_written);
    NPT_Result Seek(NPT_Position offset) {
        return NPT_SymbOSFileStream::Seek(offset);
    }
    NPT_Result Tell(NPT_Position& offset) {
        return NPT_SymbOSFileStream::Tell(offset);
    }
};

/*----------------------------------------------------------------------
|   NPT_SymbOSFileOutputStream::Write
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbOSFileOutputStream::Write(const void* buffer, 
                                  NPT_Size    bytes_to_write, 
                                  NPT_Size*   bytes_written)
{
    TPtrC8 ptrBuf((const unsigned char *) buffer, bytes_to_write);

    if (m_FileReference->GetFile()->Write(ptrBuf) == KErrNone) {
        if (bytes_written) *bytes_written = bytes_to_write;
        return NPT_SUCCESS;
    } else {
        if (bytes_written) *bytes_written = 0;
        return NPT_ERROR_WRITE_FAILED;
    }
}

/*----------------------------------------------------------------------
|   NPT_SymbOSFile
+---------------------------------------------------------------------*/
class NPT_SymbOSFile: public NPT_FileInterface
{
public:
    // constructors and destructor
    NPT_SymbOSFile(const char* name);
   ~NPT_SymbOSFile();

    // NPT_FileInterface methods
    NPT_Result Open(OpenMode mode);
    NPT_Result Close();
    NPT_Result GetSize(NPT_Size& size);
    NPT_Result GetInputStream(NPT_InputStreamReference& stream);
    NPT_Result GetOutputStream(NPT_OutputStreamReference& stream);

private:
    // members
    NPT_String            m_Name;
    OpenMode              m_Mode;
    NPT_SymbOSFileReference m_FileReference;
    NPT_Size              m_Size;
};

/*----------------------------------------------------------------------
|   NPT_SymbOSFile::NPT_SymbOSFile
+---------------------------------------------------------------------*/
NPT_SymbOSFile::NPT_SymbOSFile(const char* name) :
    m_Name(name),
    m_Mode(0),
    m_Size(0)
{
}

/*----------------------------------------------------------------------
|   NPT_SymbOSFile::~NPT_SymbOSFile
+---------------------------------------------------------------------*/
NPT_SymbOSFile::~NPT_SymbOSFile()
{
    Close();
}

/*----------------------------------------------------------------------
|   NPT_SymbOSFile::Open
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbOSFile::Open(NPT_File::OpenMode mode)
{
    // check if we're already open
    if (!m_FileReference.IsNull()) {
        return NPT_ERROR_FILE_ALREADY_OPEN;
    }

    RFs     *fsSession = new RFs();
    RFile   *rfile = new RFile();

    // store the mode
    m_Mode = mode;

    TUint len = m_Name.GetLength();
    TText* text_name = new TText [len];
    for (int i=0; i < len; ++i) {
        if (m_Name[i] == '/')
            text_name[i] = '\\';
        else
            text_name[i] = m_Name[i];
    }
    
    RBuf symb_name;
    symb_name.Assign(text_name, len, len);

    /* compute mode */
    TUint symbMode;
    if (mode & NPT_FILE_OPEN_MODE_WRITE) {
        symbMode = EFileWrite | EFileRead;
    } else {
        /* read only */
        symbMode = EFileRead;
    }

    TInt res = KErrNone;
    /* start the file session */
    res = fsSession->Connect();
    if (res != KErrNone) {
        goto cleanup;
    }

    /* try to open the file */
    if (mode & NPT_FILE_OPEN_MODE_CREATE) {
        res = rfile->Create(*fsSession, symb_name, symbMode);
    }
    else if (mode & NPT_FILE_OPEN_MODE_TRUNCATE) {
        res = rfile->Replace(*fsSession, symb_name, symbMode);
    }
    
    /* if appropos, mimic the behavior of ANSI C fopen with truncate specified */
    if ( !(mode & NPT_FILE_OPEN_MODE_TRUNCATE) || res == KErrNone ) {
        res = rfile->Open(*fsSession, symb_name, symbMode);
    }
    
    if (res != KErrNone) {
        cleanup:
        delete rfile;
        fsSession->Close();
        delete fsSession;
        return NULL;
    }
    
    int filesize = 0;
    rfile->Size(filesize);
    m_Size = filesize;
    
    // create a reference to the RFile and RFs objects
    m_FileReference = new NPT_SymbOSFileWrapper(fsSession, rfile);

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbOSFile::Close
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbOSFile::Close()
{
    // release the file reference
    m_FileReference = NULL;

    // reset the mode
    m_Mode = 0;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbOSFile::GetSize
+---------------------------------------------------------------------*/
NPT_Result 
NPT_SymbOSFile::GetSize(NPT_Size& size)
{
    // default value
    size = 0;

    // check that the file is open
    if (m_FileReference.IsNull()) return NPT_ERROR_FILE_NOT_OPEN;

    // return the size
    size = m_Size;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbOSFile::GetInputStream
+---------------------------------------------------------------------*/
NPT_Result 
NPT_SymbOSFile::GetInputStream(NPT_InputStreamReference& stream)
{
    // default value
    stream = NULL;

    // check that the file is open
    if (m_FileReference.IsNull()) return NPT_ERROR_FILE_NOT_OPEN;

    // check that the mode is compatible
    if (!(m_Mode & NPT_FILE_OPEN_MODE_READ)) {
        return NPT_ERROR_FILE_NOT_READABLE;
    }

    // create a stream
    stream = new NPT_SymbOSFileInputStream(m_FileReference, m_Size);

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbOSFile::GetOutputStream
+---------------------------------------------------------------------*/
NPT_Result 
NPT_SymbOSFile::GetOutputStream(NPT_OutputStreamReference& stream)
{
    // default value
    stream = NULL;

    // check that the file is open
    if (m_FileReference.IsNull()) return NPT_ERROR_FILE_NOT_OPEN;

    // check that the mode is compatible
    if (!(m_Mode & NPT_FILE_OPEN_MODE_WRITE)) {
        return NPT_ERROR_FILE_NOT_WRITABLE;
    }
    
    // create a stream
    stream = new NPT_SymbOSFileOutputStream(m_FileReference);

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_File::NPT_File
+---------------------------------------------------------------------*/
NPT_File::NPT_File(const char* name)
{
    m_Delegate = new NPT_SymbOSFile(name);
}
