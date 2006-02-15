#######################################################################
#
#    Core Module Makefile
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

#######################################################################
# sources and object files
#######################################################################
NPT_MODULE_SOURCES =          \
	NptStreams.cpp 		      \
	NptCommon.cpp			  \
	NptDebug.cpp			  \
	NptStrings.cpp			  \
	NptMessaging.cpp		  \
	NptRingBuffer.cpp		  \
	NptSockets.cpp			  \
	NptNetwork.cpp			  \
	NptSystem.cpp			  \
	NptSimpleMessageQueue.cpp \
	NptThreads.cpp			  \
	NptTime.cpp			      \
	NptXml.cpp			      \
	NptUri.cpp		      	  \
	NptHttp.cpp			      \
	NptUtils.cpp		      \
	NptDataBuffer.cpp		  \
	NptBufferedStreams.cpp	  \
	NptBase64.cpp

NPT_MODULE_OBJECTS = $(NPT_MODULE_SOURCES:.cpp=.o)
NPT_INCLUDES_CPP  += -I$(NPT_SOURCE_ROOT)/Neptune -I$(NPT_SOURCE_ROOT)/Core -I$(NPT_SOURCE_ROOT)/Config

#######################################################################
# paths
#######################################################################
VPATH += $(NPT_SOURCE_ROOT)/Core

#######################################################################
# clean
#######################################################################
NPT_LOCAL_FILES_TO_CLEAN = *.d *.o *.a

#######################################################################
# targets
#######################################################################
libNptCore.a: $(NPT_MODULE_OBJECTS) $(NPT_MODULE_LIBRARIES)

