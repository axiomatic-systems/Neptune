#######################################################################
#
#    Makefile Variables for the x86-unknown-linux targets
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

##########################################################################
#    tools
##########################################################################
NPT_COMPILE_CPP   = g++
NPT_FLAGS_CPP     = -ansi -pedantic
NPT_DEFINES_CPP   = -D_REENTRANT -D_BSD_SOURCE -D_POSIX_SOURCE
NPT_WARNINGS_CPP  = -Werror -Wall -W -Wundef -Wcast-qual -Wconversion -Wimplicit -Woverloaded-virtual 
NPT_AUTODEP_CPP   = g++ -MM
NPT_LINK_CPP      = g++
NPT_LIBRARIES_CPP = -lpthread

NPT_ARCHIVE_O     = ld -r -o
NPT_ARCHIVE_A     = ar rs

NPT_COPY_IF_NEW   = cp -u
NPT_MAKE_FLAGS    = --no-print-directory

### dmalloc support
ifneq ($(DMALLOC_OPTIONS),)
NPT_DEFINES_C     += -DDMALLOC
NPT_LIBRARIES_CPP += -ldmallocthcxx
NPT_FLAGS_CPP      = -ansi
endif
