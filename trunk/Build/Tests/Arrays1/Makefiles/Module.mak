#######################################################################
#
#    Module Makefile
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

#######################################################################
# module dependencies
#######################################################################
NPT_IMPORT_MODULES = Neptune
include $(NPT_BUILD_INCLUDES)/ImportModuleDeps.mak

#######################################################################
# sources and object files
#######################################################################
NPT_MODULE_SOURCES = ArraysTest1.cpp
NPT_MODULE_OBJECTS = $(NPT_MODULE_SOURCES:.cpp=.o)

#######################################################################
# paths
#######################################################################
VPATH += $(NPT_SOURCE_ROOT)/Tests/Arrays1

#######################################################################
# clean
#######################################################################
NPT_LOCAL_FILES_TO_CLEAN = *.d *.o *.a ArraysTest1

#######################################################################
# targets
#######################################################################
ArraysTest1: $(NPT_IMPORTED_MODULE_LIBS) $(NPT_MODULE_OBJECTS) $(NPT_MODULE_LIBRARIES)
	$(NPT_MAKE_BANNER_START)
	$(NPT_MAKE_EXECUTABLE_COMMAND_CPP)
	$(NPT_MAKE_BANNER_END)
