#######################################################################
#
#    Makefile Variables
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

##########################################################################
#    tools
##########################################################################
NPT_COMPILE_CPP   = pacc -I"$(PALMOS_SDK_ROOT)/headers" -I"$(PALMOS_SDK_ROOT)/headers/posix" -I"$(PALMOS_TOOLS_ROOT)/include/stlport"
NPT_FLAGS_CPP     = -D__PALMOS__=0x06000000 -D__PALMOS_KERNEL__=1
NPT_DEFINES_CPP   = 
NPT_WARNINGS_CPP  = -W2
NPT_AUTODEP_CPP   = g++ -MM
NPT_LINK_CPP      = pacc
NPT_LIBRARIES_CPP = 

NPT_ARCHIVE_O     = ld -r -o
NPT_ARCHIVE_A     = ar rs

NPT_COPY_IF_NEW   = cp -u
NPT_MAKE_FLAGS    = --no-print-directory
