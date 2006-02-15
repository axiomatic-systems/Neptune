#######################################################################
#
#    System Module Makefile
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

#######################################################################
# module dependencies
#######################################################################
NPT_INCLUDE_MODULES = Core
include $(NPT_BUILD_INCLUDES)/IncludeModuleDeps.mak

#######################################################################
# sources and object files
#######################################################################
ifneq ($(NPT_SYSTEM_DEBUG_IMPLEMENTATION),)
	NPT_MODULE_SOURCES += Npt$(NPT_SYSTEM_DEBUG_IMPLEMENTATION).cpp
endif
ifneq ($(NPT_SYSTEM_SYSTEM_IMPLEMENTATION),)
	NPT_MODULE_SOURCES += Npt$(NPT_SYSTEM_SYSTEM_IMPLEMENTATION).cpp
endif
ifneq ($(NPT_SYSTEM_SOCKETS_IMPLEMENTATION),)
	NPT_MODULE_SOURCES += Npt$(NPT_SYSTEM_SOCKETS_IMPLEMENTATION).cpp
endif
ifneq ($(NPT_SYSTEM_NETWORK_IMPLEMENTATION),)
	NPT_MODULE_SOURCES += Npt$(NPT_SYSTEM_NETWORK_IMPLEMENTATION).cpp
endif
ifneq ($(NPT_SYSTEM_NETWORK_IMPLEMENTATION),)
	NPT_MODULE_SOURCES += Npt$(NPT_SYSTEM_NETWORK_IMPLEMENTATION).cpp
endif
ifneq ($(NPT_SYSTEM_THREADS_IMPLEMENTATION),)
	NPT_MODULE_SOURCES += Npt$(NPT_SYSTEM_THREADS_IMPLEMENTATION).cpp
endif
ifneq ($(NPT_SYSTEM_QUEUE_IMPLEMENTATION),)
	NPT_MODULE_SOURCES += Npt$(NPT_SYSTEM_QUEUE_IMPLEMENTATION).cpp
endif
ifneq ($(NPT_SYSTEM_FILE_IMPLEMENTATION),)
	NPT_MODULE_SOURCES += Npt$(NPT_SYSTEM_FILE_IMPLEMENTATION).cpp
endif

ifneq ($(findstring SelectableMessageQueue, $(NPT_SYSTEM_OPTIONS)),)
NPT_MODULE_SOURCES += NptSelectableMessageQueue.cpp
endif


NPT_MODULE_OBJECTS = $(NPT_MODULE_SOURCES:.cpp=.o)

#######################################################################
# paths
#######################################################################
VPATH += $(NPT_SOURCE_ROOT)/System/Bsd:$(NPT_SOURCE_ROOT)/System/StdC:$(NPT_SOURCE_ROOT)/System/Posix:$(NPT_SOURCE_ROOT)/System/Win32:$(NPT_SOURCE_ROOT)/System/Null 

#######################################################################
# clean
#######################################################################
NPT_LOCAL_FILES_TO_CLEAN = *.d *.o *.a

#######################################################################
# targets
#######################################################################
libNptSystem.a: $(NPT_MODULE_OBJECTS)

