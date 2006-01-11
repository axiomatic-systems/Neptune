#######################################################################
#
#    Local Target Makefile
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

#######################################################################
#    target specific files
#######################################################################
NPT_SYSTEM_DEBUG_IMPLEMENTATION   = Win32Debug
NPT_SYSTEM_SYSTEM_IMPLEMENTATION  = NullSystem
NPT_SYSTEM_SOCKETS_IMPLEMENTATION = BsdSockets
NPT_SYSTEM_FILE_IMPLEMENTATION    = StdcFileByteStream
NPT_SYSTEM_THREADS_IMPLEMENTATION = Win32Threads
NPT_SYSTEM_QUEUE_IMPLEMENTATION   = Win32Queue
