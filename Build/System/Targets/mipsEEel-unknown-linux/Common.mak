#######################################################################
#
#    Common Target Makefile
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################
all: libNptSystem.a

#######################################################################
#    configuration variables
#######################################################################
NPT_TARGET = mipsEEel-unknown-linux
NPT_ROOT   = ../../../../..

#######################################################################
#    target specific files
#######################################################################
NPT_SYSTEM_DEBUG_IMPLEMENTATION   = StdcDebug
NPT_SYSTEM_SYSTEM_IMPLEMENTATION  = PosixSystem
NPT_SYSTEM_SOCKETS_IMPLEMENTATION = BsdSockets
NPT_SYSTEM_FILE_IMPLEMENTATION    = StdcFileByteStream
NPT_SYSTEM_THREADS_IMPLEMENTATION = PosixThreads
NPT_SYSTEM_QUEUE_IMPLEMENTATION   = PosixQueue

#######################################################################
#    includes
#######################################################################
include $(NPT_ROOT)/Build/Includes/TopLevel.mak
include ../../../Makefiles/Module.mak
include $(NPT_BUILD_INCLUDES)/BuildConfig.mak
include $(NPT_BUILD_INCLUDES)/AutoDep.mak
include $(NPT_BUILD_INCLUDES)/Rules.mak
