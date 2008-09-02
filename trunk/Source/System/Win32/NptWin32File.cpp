/*****************************************************************
|
|      Neptune - File :: Win32 Implementation
|
|      (c) 2001-2008 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "NptLogging.h"
#include "NptFile.h"
#include "NptUtils.h"

#include <windows.h>

/*----------------------------------------------------------------------
|   logging
+---------------------------------------------------------------------*/
//NPT_SET_LOCAL_LOGGER("neptune.win32.file")

/*----------------------------------------------------------------------
|   MapError
+---------------------------------------------------------------------*/
static NPT_Result
MapError(DWORD err) {
    switch (err) {
      case ERROR_ALREADY_EXISTS:    return NPT_ERROR_FILE_ALREADY_EXISTS;
      case ERROR_PATH_NOT_FOUND:    return NPT_ERROR_NO_SUCH_FILE;
      case ERROR_FILE_NOT_FOUND:    return NPT_ERROR_NO_SUCH_FILE;
      case ERROR_ACCESS_DENIED:     return NPT_ERROR_PERMISSION_DENIED;
      case ERROR_SHARING_VIOLATION: return NPT_ERROR_FILE_BUSY;
      default:                      return NPT_FAILURE;
    }
}

/*----------------------------------------------------------------------
|   NPT_FilePath::Separator
+---------------------------------------------------------------------*/
const NPT_String NPT_FilePath::Separator("\\s");

/*----------------------------------------------------------------------
|   NPT_File::GetRoots
+---------------------------------------------------------------------*/
NPT_Result
NPT_File::GetRoots(NPT_List<NPT_String>& roots)
{
    roots.Clear();
    DWORD drives = GetLogicalDrives();
    for (unsigned int i=0; i<26; i++) {
        if (drives & (1<<i)) {
            char drive_name[3] = {'A'+i, ':', 0};
            roots.Add(drive_name);
        }
    }
    return NPT_ERROR_NOT_SUPPORTED;
}

/*----------------------------------------------------------------------
|   NPT_File_ProcessFindData
+---------------------------------------------------------------------*/
static void
NPT_File_ProcessFindData(WIN32_FIND_DATA* find_data, NPT_List<NPT_String>& entries)
{
    entries.Add(find_data->cFileName);
}

/*----------------------------------------------------------------------
|   NPT_File::ListDirectory
+---------------------------------------------------------------------*/
NPT_Result 
NPT_File::ListDirectory(const char* path, NPT_List<NPT_String>& entries)
{
    // default return value
    entries.Clear();

    // check the arguments
    if (path == NULL) return NPT_ERROR_INVALID_PARAMETERS;

    // list the entries
    WIN32_FIND_DATA find_data;
    HANDLE find_handle = FindFirstFile(path, &find_data);
    if (find_handle == INVALID_HANDLE_VALUE) return MapError(GetLastError());
    NPT_File_ProcessFindData(&find_data, entries);
    while (FindNextFile(find_handle, &find_data)) {
        NPT_File_ProcessFindData(&find_data, entries);
    }
    DWORD last_error = GetLastError();
    FindClose(find_handle);
    if (last_error != ERROR_NO_MORE_FILES) return MapError(last_error);

    return NPT_SUCCESS;
}