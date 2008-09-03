/*****************************************************************
|
|      Neptune - File :: Posix Implementation
|
|      (c) 2001-2008 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#define _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE64
#define _FILE_OFFSET_BITS 64

#include <sys/stat.h>

#include <errno.h>

#if defined(_WIN32)
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif

#include "NptLogging.h"
#include "NptFile.h"
#include "NptUtils.h"

/*----------------------------------------------------------------------
|   logging
+---------------------------------------------------------------------*/
//NPT_SET_LOCAL_LOGGER("neptune.posix.file")

/*----------------------------------------------------------------------
|   Win32 adaptation
+---------------------------------------------------------------------*/
#if defined(_WIN32)
#define mkdir(_path,_mode) _mkdir(_path)
#define getcwd _getcwd
#define unlink _unlink
#define rmdir  _rmdir
#define S_ISDIR(_m) (((_m)&_S_IFMT) == _S_IFDIR) 
#define S_ISREG(_m) (((_m)&_S_IFMT) == _S_IFREG) 
#define S_IWUSR _S_IWRITE
#endif

/*----------------------------------------------------------------------
|   MapErrno
+---------------------------------------------------------------------*/
static NPT_Result
MapErrno(int err) {
    switch (err) {
      case EACCES:       return NPT_ERROR_PERMISSION_DENIED;
      case EPERM:        return NPT_ERROR_PERMISSION_DENIED;
      case ENOENT:       return NPT_ERROR_NO_SUCH_FILE;
      case ENAMETOOLONG: return NPT_ERROR_INVALID_PARAMETERS;
      case EBUSY:        return NPT_ERROR_FILE_BUSY;
      case EROFS:        return NPT_ERROR_FILE_NOT_WRITABLE;
      case ENOTDIR:      return NPT_ERROR_FILE_NOT_DIRECTORY;
      case EEXIST:       return NPT_ERROR_FILE_ALREADY_EXISTS;
      case ENOSPC:       return NPT_ERROR_FILE_NOT_ENOUGH_SPACE;
      case ENOTEMPTY:    return NPT_ERROR_DIRECTORY_NOT_EMPTY;
      default:           return NPT_ERROR_ERRNO(err);
    }
}

/*----------------------------------------------------------------------
|   NPT_FilePath::Separator
+---------------------------------------------------------------------*/
#if !defined(_WIN32)
const NPT_String NPT_FilePath::Separator("/");
#endif

#if !defined(_WIN32)
/*----------------------------------------------------------------------
|   NPT_File::GetRoots
+---------------------------------------------------------------------*/
NPT_Result
NPT_File::GetRoots(NPT_List<NPT_String>& roots)
{
    roots.Clear();
    roots.Add("/");
    return NPT_SUCCESS;
}
#endif

/*----------------------------------------------------------------------
|   NPT_File::CreateDirectory
+---------------------------------------------------------------------*/
NPT_Result
NPT_File::CreateDirectory(const char* path)
{
    int result;
    
    result = mkdir(path, 0755);
    if (result != 0) {
        return MapErrno(errno);
    }
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_File::GetWorkingDirectory
+---------------------------------------------------------------------*/
NPT_Result
NPT_File::GetWorkingDirectory(NPT_String& path)
{
    char* buffer = new char[1024+1];
    char* dir = getcwd(buffer, 1024+1);
    if (dir == NULL) return MapErrno(errno);
    path = dir;
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_File::GetInfo
+---------------------------------------------------------------------*/
NPT_Result
NPT_File::GetInfo(const char* path, NPT_FileInfo* info)
{
    // default value
    if (info) NPT_SetMemory(info, 0, sizeof(*info));
    
    // get the file info
    NPT_stat_struct stat_buffer;
    int result = NPT_stat(path, &stat_buffer);
    if (result != 0) return MapErrno(errno);
    
    // setup the returned fields
    if (info) {
        info->m_Size = stat_buffer.st_size;
        if (S_ISREG(stat_buffer.st_mode)) {
            info->m_Type = NPT_FileInfo::FILE_TYPE_REGULAR;
        } else if (S_ISDIR(stat_buffer.st_mode)) {
            info->m_Type = NPT_FileInfo::FILE_TYPE_DIRECTORY;
        } else {
            info->m_Type = NPT_FileInfo::FILE_TYPE_OTHER;
        }
        info->m_AttributesMask &= NPT_FILE_ATTRIBUTE_READ_ONLY;
        if ((stat_buffer.st_mode & S_IWUSR) == 0) {
            info->m_Attributes &= NPT_FILE_ATTRIBUTE_READ_ONLY;
        }
    }
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_File::DeleteFile
+---------------------------------------------------------------------*/
NPT_Result
NPT_File::DeleteFile(const char* path)
{
    int result = unlink(path);
    if (result != 0) return MapErrno(errno);
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_File::DeleteDirectory
+---------------------------------------------------------------------*/
NPT_Result
NPT_File::DeleteDirectory(const char* path)
{
    int result = rmdir(path);
    if (result != 0) return MapErrno(errno);
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_File::Rename
+---------------------------------------------------------------------*/
NPT_Result
NPT_File::Rename(const char* from_path, const char* to_path)
{
    int result = rename(from_path, to_path);
    if (result != 0) return MapErrno(errno);
    
    return NPT_SUCCESS;
}
