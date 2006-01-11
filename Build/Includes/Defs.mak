#######################################################################
#
#    Build Definitions
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

#######################################################################
# variables
#######################################################################
XXX_CLIENT                = NPT
NPT_BUILD_ROOT            = $(NPT_ROOT)/Build
NPT_BUILD_INCLUDES        = $(NPT_BUILD_ROOT)/Includes
NPT_BUILD_MODULES         = $(NPT_BUILD_INCLUDES)/Modules
NPT_BUILD_TARGET_INCLUDES = $(NPT_BUILD_INCLUDES)/Targets/$(NPT_TARGET)
NPT_BUILD_CONFIG_INCLUDES = $(NPT_BUILD_TARGET_INCLUDES)/BuildConfigs/$(NPT_BUILD_CONFIG)
NPT_SOURCE_ROOT    = $(NPT_ROOT)/Source
ifndef NPT_BUILD_CONFIG
NPT_BUILD_CONFIG = Debug
endif
