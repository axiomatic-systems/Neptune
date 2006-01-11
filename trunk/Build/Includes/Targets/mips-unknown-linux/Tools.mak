#######################################################################
#
#    Makefile Variables for the mips-unknown-linux targets
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

##########################################################################
#    tools
##########################################################################
NPT_AUTODEP_STDOUT = true

NPT_COMPILE_CPP   = g++ -msoft-float -mips2
NPT_FLAGS_CPP     = -ansi -pedantic
NPT_DEFINES_CPP   = -D_REENTRANT -D_BSD_SOURCE -D_POSIX_SOURCE -D_XOPEN_SOURCE -D_POSIX_C_SOURCE=199506
NPT_WARNINGS_CPP  = -Werror -Wall -W -Wcast-qual -Wconversion -Wimplicit -Woverloaded-virtual 
NPT_AUTODEP_CPP   = g++ -MM
NPT_LINK_CPP      = g++ -msoft-float -mips2
NPT_LIBRARIES_CPP = -lpthread

NPT_ARCHIVE_O     = ld -r -o
NPT_ARCHIVE_A     = ar rs

NPT_COPY_IF_NEW   = cp 
NPT_MAKE_FLAGS    = --no-print-directory

### dmalloc support
ifneq ($(DMALLOC_OPTIONS),)
NPT_DEFINES_C     += -DDMALLOC
NPT_LIBRARIES_CPP += -ldmallocthcxx
NPT_FLAGS_CPP      = -ansi
endif
