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
NPT_COMPILE_CPP   = tmcc
NPT_FLAGS_CPP     = -target tm1 -el -bcompact -bremoveunusedcode
NPT_DEFINES_CPP   = -D_REENTRANT 
NPT_WARNINGS_CPP  = 
NPT_AUTODEP_CPP   = g++ -MM
NPT_LINK_CPP      = tmcc -target tm1 -el -s
NPT_LIBRARIES_CPP = -lC++

#NPT_ARCHIVE_O     = tmcc -partial -el -o 
NPT_ARCHIVE_A     = tmcc -partial -el -o 
NPT_ARCHIVE_O     = tmar -rv  
#NPT_ARCHIVE_A     = tmar -rv 

NPT_COPY_IF_NEW   = cp -u
NPT_MAKE_FLAGS    = --no-print-directory

