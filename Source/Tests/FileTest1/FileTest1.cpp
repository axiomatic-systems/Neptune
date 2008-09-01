/*****************************************************************
|
|      File Test Program 1
|
|      (c) 2005-2008 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Neptune.h"
#include "NptDebug.h"

/*----------------------------------------------------------------------
|    CreateNewFile
+---------------------------------------------------------------------*/
NPT_Result
CreateNewFile(const char* filename, NPT_Size size)
{
    NPT_File file(filename);
    NPT_CHECK(file.Open(NPT_FILE_OPEN_MODE_CREATE|NPT_FILE_OPEN_MODE_WRITE));
    NPT_OutputStreamReference out;
    file.GetOutputStream(out);
    for (unsigned int i=0; i<size; i++) {
        char c = (char)i;
        NPT_ASSERT(NPT_SUCCEEDED(out->WriteFully(&c, 1))); 
    }
    file.Close();
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int
main(int /*argc*/, char** /*argv*/)
{
    NPT_Result result;
    NPT_FileInfo info;
    
    NPT_ASSERT(NPT_File::GetInfo("foobar.doesnotexist", NULL) == NPT_ERROR_NO_SUCH_FILE);
    NPT_ASSERT(!NPT_File::Exists("foobar.doesnotexist"));
    
    // test special names
    NPT_File file(NPT_FILE_STANDARD_INPUT);
    NPT_ASSERT(NPT_SUCCEEDED(file.GetInfo(info)));
    NPT_ASSERT(info.m_Type == NPT_FileInfo::FILE_TYPE_SPECIAL);
    file = NPT_File(NPT_FILE_STANDARD_OUTPUT);
    
    NPT_ASSERT(NPT_SUCCEEDED(file.GetInfo(info)));
    NPT_ASSERT(info.m_Type == NPT_FileInfo::FILE_TYPE_SPECIAL);
    file = NPT_File(NPT_FILE_STANDARD_ERROR);
    NPT_ASSERT(NPT_SUCCEEDED(file.GetInfo(info)));
    NPT_ASSERT(info.m_Type == NPT_FileInfo::FILE_TYPE_SPECIAL);

    if (NPT_File::Exists("foobar.file1")) {
        result = NPT_File::DeleteFile("foobar.file1");
        NPT_ASSERT(NPT_SUCCEEDED(result));
    }
    
    result = CreateNewFile("foobar.file1", 9);
    NPT_ASSERT(NPT_SUCCEEDED(result));
    NPT_ASSERT(NPT_File::Exists("foobar.file1"));
    result = NPT_File::GetInfo("foobar.file1", &info);
    NPT_ASSERT(NPT_SUCCEEDED(result));
    NPT_ASSERT(info.m_Type == NPT_FileInfo::FILE_TYPE_REGULAR);
    NPT_ASSERT(info.m_Size == 9);
    
    {
        NPT_File f1("foobar.file1");
        result = f1.GetInfo(info);
        NPT_ASSERT(NPT_SUCCEEDED(result));
        NPT_ASSERT(info.m_Type == NPT_FileInfo::FILE_TYPE_REGULAR);
        NPT_ASSERT(info.m_Size == 9);
    }
    {
        NPT_File f1("foobar.file1");
        NPT_Size size;
        result = f1.GetSize(size);
        NPT_ASSERT(NPT_SUCCEEDED(result));
        NPT_ASSERT(size == 9);
    }
    
    {
        NPT_File f1("foobar.file1");
        result = f1.Rename("foobar.file1-r");
        NPT_ASSERT(NPT_SUCCEEDED(result));
        NPT_ASSERT(f1.GetPath() == "foobar.file1-r");
    }
    NPT_ASSERT(NPT_File::Exists("foobar.file1-r"));
    result = NPT_File::GetInfo("foobar.file1-r", &info);
    NPT_ASSERT(NPT_SUCCEEDED(result));
    NPT_ASSERT(info.m_Type == NPT_FileInfo::FILE_TYPE_REGULAR);
    NPT_ASSERT(info.m_Size == 9);

    // dirs
    NPT_ASSERT(!NPT_File::Exists("foobar.dir"));
    result = NPT_File::CreateDirectory("foobar.dir");
    NPT_ASSERT(NPT_SUCCEEDED(result));
    result = NPT_File::GetInfo("foobar.dir", &info);
    NPT_ASSERT(NPT_SUCCEEDED(result));
    NPT_ASSERT(info.m_Type == NPT_FileInfo::FILE_TYPE_DIRECTORY);
    {
        NPT_File f1("foobar.dir");
        result = f1.GetInfo(info);
        NPT_ASSERT(NPT_SUCCEEDED(result));
        NPT_ASSERT(info.m_Type == NPT_FileInfo::FILE_TYPE_DIRECTORY);
    }

    NPT_String dirname = "foobar.dir";
    NPT_String fname;
    fname = dirname;
    fname += NPT_FilePath::Separator;
    fname += "file1";
    result = CreateNewFile(fname, 1);
    NPT_ASSERT(NPT_SUCCEEDED(result));
    fname = dirname;
    fname += NPT_FilePath::Separator;
    fname += "file2";
    result = CreateNewFile(fname, 2);
    NPT_ASSERT(NPT_SUCCEEDED(result));
    fname = dirname;
    fname += NPT_FilePath::Separator;
    fname += "file3";
    result = CreateNewFile(fname, 3);
    NPT_ASSERT(NPT_SUCCEEDED(result));

    NPT_List<NPT_String> entries;
    result = NPT_File::ListDirectory("foobar.dir", entries);
    NPT_ASSERT(NPT_SUCCEEDED(result));
    NPT_ASSERT(entries.GetItemCount() == 3);
    
    result = NPT_File::DeleteFile("foobar.dir");
    NPT_ASSERT(NPT_FAILED(result));
    result = NPT_File::DeleteDirectory("foobar.dir");
    NPT_ASSERT(result == NPT_ERROR_DIRECTORY_NOT_EMPTY);
    
    result = NPT_File::Rename("foobar.dir", "foobar.dir-r");
    NPT_ASSERT(NPT_SUCCEEDED(result));
    
    dirname = "foobar.dir-r";
    fname = dirname;
    fname += NPT_FilePath::Separator;
    fname += "file1";
    result = NPT_File::DeleteFile(fname);
    NPT_ASSERT(NPT_SUCCEEDED(result));
    fname = dirname;
    fname += NPT_FilePath::Separator;
    fname += "file2";
    result = NPT_File::DeleteFile(fname);
    NPT_ASSERT(NPT_SUCCEEDED(result));
    fname = dirname;
    fname += NPT_FilePath::Separator;
    fname += "file3";
    result = NPT_File::DeleteFile(fname);
    NPT_ASSERT(NPT_SUCCEEDED(result));

    result = NPT_File::DeleteDirectory("foobar.dir-r");
    NPT_ASSERT(NPT_SUCCEEDED(result));
    NPT_ASSERT(!NPT_File::Exists("foobar.dir-r"));

    // paths
    NPT_String test;
    test = NPT_FilePath::BaseName("");
    NPT_ASSERT(test == "");
    test = NPT_FilePath::BaseName("a");
    NPT_ASSERT(test == "a");
    test = NPT_FilePath::BaseName("a"+NPT_FilePath::Separator+"b");
    NPT_ASSERT(test == "b");
    test = NPT_FilePath::BaseName("a"+NPT_FilePath::Separator+"b"+NPT_FilePath::Separator);
    NPT_ASSERT(test == "");
    test = NPT_FilePath::BaseName(NPT_FilePath::Separator+"a");
    NPT_ASSERT(test == "a");
    test = NPT_FilePath::BaseName(NPT_FilePath::Separator);
    NPT_ASSERT(test == "");

    test = NPT_FilePath::DirectoryName("");
    NPT_ASSERT(test == "");
    test = NPT_FilePath::DirectoryName("a");
    NPT_ASSERT(test == "");
    test = NPT_FilePath::DirectoryName("a"+NPT_FilePath::Separator+"b");
    NPT_ASSERT(test == "a");
    test = NPT_FilePath::DirectoryName("a"+NPT_FilePath::Separator+"b"+NPT_FilePath::Separator);
    NPT_ASSERT(test == "a"+NPT_FilePath::Separator+"b");
    test = NPT_FilePath::DirectoryName(NPT_FilePath::Separator+"a");
    NPT_ASSERT(test == NPT_FilePath::Separator);
    test = NPT_FilePath::DirectoryName(NPT_FilePath::Separator);
    NPT_ASSERT(test == NPT_FilePath::Separator);
    
    return 0;
}
