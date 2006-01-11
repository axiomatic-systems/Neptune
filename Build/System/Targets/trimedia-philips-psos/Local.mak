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
NPT_SYSTEM_SYSTEM_IMPLEMENTATION  = NullSystem
NPT_SYSTEM_SOCKETS_IMPLEMENTATION = BsdSockets
NPT_SYSTEM_FILE_IMPLEMENTATION    = StdcFile
NPT_SYSTEM_THREADS_IMPLEMENTATION = NullThreads
NPT_SYSTEM_QUEUE_IMPLEMENTATION   = NullQueue

NPT_INCLUDES_CPP += -Ic:/Develop/StreamIOm.ms3/IADK/PNX1300/net/comps/targettcp/inc/
VPATH += $(NPT_SOURCE_ROOT)/System/Streamium
