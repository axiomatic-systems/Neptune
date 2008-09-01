/*****************************************************************
|
|      Neptune - Dynamic Libraries :: Posix Implementation
|
|      (c) 2001-2008 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "NptLogging.h"
#include "NptDynamicLibraries.h"

#include <dlfcn.h>

/*----------------------------------------------------------------------
|   logging
+---------------------------------------------------------------------*/
NPT_SET_LOCAL_LOGGER("neptune.posix.dynamic-libraries")

/*----------------------------------------------------------------------
|   NPT_PosixDynamicLibrary
+---------------------------------------------------------------------*/
class NPT_PosixDynamicLibrary : public NPT_DynamicLibraryInterface
{
public:
    // constructor and destructor
    NPT_PosixDynamicLibrary(void* library, const char* name) : 
        m_Library(library), m_Name(name) {}
    
    // NPT_DynamicLibraryInterface methods
    virtual NPT_Result FindSymbol(const char* name, void*& symbol);
    virtual NPT_Result Unload();
    
private:
    // members
    void*      m_Library;
    NPT_String m_Name;
};

/*----------------------------------------------------------------------
|   NPT_DynamicLibrary::Load
+---------------------------------------------------------------------*/
NPT_Result 
NPT_DynamicLibrary::Load(const char* name, NPT_Flags flags, NPT_DynamicLibrary*& library)
{
    if (name == NULL) return NPT_ERROR_INVALID_PARAMETERS;
    
    // default return value
    library = NULL;
    
    // compute the mode
    int mode = 0;
    if (flags & NPT_DYANMIC_LIBRARY_LOAD_FLAG_NOW) {
        mode &= RTLD_NOW;
    } else {
        mode &= RTLD_LAZY;
    }
    
    // load the lib
    NPT_LOG_FINE_2("loading library %s, flags=%x", name, flags);
    void* handle = dlopen(name, mode);
    if (handle == NULL) {
        NPT_LOG_FINE("library not found");
        return NPT_FAILURE;
    }
    
    // instantiate the object
    NPT_LOG_FINE_1("library %s loaded", name);
    library = new NPT_DynamicLibrary(new NPT_PosixDynamicLibrary(handle, name));
    
    return NPT_SUCCESS;
}
    
/*----------------------------------------------------------------------
|   NPT_PosixDynamicLibrary::FindSymbol
+---------------------------------------------------------------------*/
NPT_Result 
NPT_PosixDynamicLibrary::FindSymbol(const char* name, void*& symbol)
{
    if (name == NULL) return NPT_ERROR_INVALID_PARAMETERS;
    if (m_Library == NULL) return NULL;
    
    NPT_LOG_FINE_1("finding symbol %s", name);
    symbol = dlsym(m_Library, name);
    return symbol?NPT_SUCCESS:NPT_ERROR_NO_SUCH_ITEM;
}

/*----------------------------------------------------------------------
|   NPT_PosixDynamicLibrary::Unload
+---------------------------------------------------------------------*/
NPT_Result
NPT_PosixDynamicLibrary::Unload()
{
    NPT_LOG_FINE_1("unloading library %s", (const char*)m_Name);
    int result = dlclose(m_Library);
    if (result == 0) {
        return NPT_SUCCESS;
    } else {
        return NPT_FAILURE;
    }
}
