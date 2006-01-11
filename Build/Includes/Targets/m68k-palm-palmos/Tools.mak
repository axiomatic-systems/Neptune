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
NPT_COMPILE_CPP   = m68k-palmos-gcc 
NPT_FLAGS_CPP     = -D__PALMOS__=0x05000000 -fexceptions -palmos5r4
NPT_DEFINES_CPP   = 
NPT_WARNINGS_CPP  = -Wall
NPT_AUTODEP_CPP   = m68k-palmos-gcc -MM
NPT_LINK_CPP      = m68k-palmos-gcc
NPT_LIBRARIES_CPP = 

NPT_ARCHIVE_O     = m68k-palmos-ld -r -o
NPT_ARCHIVE_A     = m68k-palmos-ar rs

NPT_COPY_IF_NEW   = cp -u
NPT_MAKE_FLAGS    = --no-print-directory
