/*****************************************************************
|
|      Neptune - Result Codes
|
|      (c) 2001-2006 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

#ifndef _NPT_RESULTS_H_
#define _NPT_RESULTS_H_

/*----------------------------------------------------------------------
|       macros
+---------------------------------------------------------------------*/
#define NPT_CHECK(x)                \
do {                                \
    NPT_Result _result = (x);       \
    if (_result != NPT_SUCCESS) {   \
        return _result;             \
    }                               \
} while(0)

#define NPT_FAILED(result)              ((result) != NPT_SUCCESS)
#define NPT_SUCCEEDED(result)           ((result) == NPT_SUCCESS)

/*----------------------------------------------------------------------
|       result codes
+---------------------------------------------------------------------*/
/** Result indicating that the operation or call succeeded */
#define NPT_SUCCESS                     0

/** Result indicating an unspecififed failure condition */
#define NPT_FAILURE                     (-1)

#if !defined(NPT_ERROR_BASE)
#define NPT_ERROR_BASE -20000
#endif

// error bases
const int NPT_ERROR_BASE_GENERAL        = NPT_ERROR_BASE-0;
const int NPT_ERROR_BASE_LIST           = NPT_ERROR_BASE-100;
const int NPT_ERROR_BASE_FILE           = NPT_ERROR_BASE-200;
const int NPT_ERROR_BASE_IO             = NPT_ERROR_BASE-300;
const int NPT_ERROR_BASE_SOCKET         = NPT_ERROR_BASE-400;
const int NPT_ERROR_BASE_INTERFACES     = NPT_ERROR_BASE-500;
const int NPT_ERROR_BASE_XML            = NPT_ERROR_BASE-600;
const int NPT_ERROR_BASE_UNIX           = NPT_ERROR_BASE-700;
const int NPT_ERROR_BASE_HTTP           = NPT_ERROR_BASE-800;

// general errors
const int NPT_ERROR_INVALID_PARAMETERS  = NPT_ERROR_BASE_GENERAL - 2;
const int NPT_ERROR_PERMISSION_DENIED   = NPT_ERROR_BASE_GENERAL - 3;
const int NPT_ERROR_OUT_OF_MEMORY       = NPT_ERROR_BASE_GENERAL - 4;
const int NPT_ERROR_NO_SUCH_NAME        = NPT_ERROR_BASE_GENERAL - 5;
const int NPT_ERROR_NO_SUCH_PROPERTY    = NPT_ERROR_BASE_GENERAL - 6;
const int NPT_ERROR_NO_SUCH_ITEM        = NPT_ERROR_BASE_GENERAL - 7;
const int NPT_ERROR_NO_SUCH_CLASS       = NPT_ERROR_BASE_GENERAL - 8;
const int NPT_ERROR_OVERFLOW            = NPT_ERROR_BASE_GENERAL - 9;
const int NPT_ERROR_INTERNAL            = NPT_ERROR_BASE_GENERAL - 10;
const int NPT_ERROR_INVALID_STATE       = NPT_ERROR_BASE_GENERAL - 11;
const int NPT_ERROR_INVALID_FORMAT      = NPT_ERROR_BASE_GENERAL - 12;
const int NPT_ERROR_INVALID_SYNTAX      = NPT_ERROR_BASE_GENERAL - 13;
const int NPT_ERROR_NOT_IMPLEMENTED     = NPT_ERROR_BASE_GENERAL - 14;
const int NPT_ERROR_NOT_SUPPORTED       = NPT_ERROR_BASE_GENERAL - 15;
const int NPT_ERROR_TIMEOUT             = NPT_ERROR_BASE_GENERAL - 16;
const int NPT_ERROR_WOULD_BLOCK         = NPT_ERROR_BASE_GENERAL - 17;

#endif // _NPT_RESULTS_H_
