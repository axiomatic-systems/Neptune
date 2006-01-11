#######################################################################
#
#    Top Level Makefile
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

######################################################################
#    checks 
#####################################################################
ifndef NPT_ROOT
$(error Variable NPT_ROOT is not set)
endif

ifndef NPT_TARGET
$(error Variable NPT_TARGET is not set)
endif

ifndef NPT_BUILD_CONFIG
$(error Variable NPT_BUILD_CONFIG is not set)
endif

######################################################################
#    includes
#####################################################################
include $(NPT_ROOT)/Build/Includes/Defs.mak
include $(NPT_BUILD_INCLUDES)/Exports.mak
include $(NPT_BUILD_TARGET_INCLUDES)/Tools.mak
-include $(NPT_BUILD_TARGET_INCLUDES)/Config.mak
-include $(NPT_BUILD_CONFIG_INCLUDES)/Config.mak
