#######################################################################
#
#    Target Makefile
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

#######################################################################
#    configuration variables
#######################################################################
NPT_TARGET = mipsEEel-unknown-linux
NPT_ROOT   = ../../../../../..

#######################################################################
#    includes
#######################################################################
include $(NPT_ROOT)/Build/Includes/TopLevel.mak
include ../../../Makefiles/Module.mak
-include ../../../Makefiles/BuildConfigs/$(NPT_BUILD_CONFIG)/Config.mak
include $(NPT_BUILD_INCLUDES)/AutoDep.mak
include $(NPT_BUILD_INCLUDES)/Rules.mak
