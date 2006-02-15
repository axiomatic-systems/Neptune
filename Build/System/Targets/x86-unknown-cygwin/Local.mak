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
NPT_SYSTEM_DEBUG_IMPLEMENTATION   = StdcDebug
NPT_SYSTEM_SYSTEM_IMPLEMENTATION  = PosixSystem
NPT_SYSTEM_SOCKETS_IMPLEMENTATION = BsdSockets
NPT_SYSTEM_NETWORK_IMPLEMENTATION = BsdNetwork
NPT_SYSTEM_FILE_IMPLEMENTATION    = StdcFile
NPT_SYSTEM_THREADS_IMPLEMENTATION = PosixThreads
NPT_SYSTEM_QUEUE_IMPLEMENTATION   = PosixQueue
