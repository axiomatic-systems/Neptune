#######################################################################
#
#    Module Exports
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

ifeq ($(NPT_NEPTUNE_MODULE_INCLUDED),)
NPT_NEPTUNE_MODULE_INCLUDED := yes

##########################################################################
# include dependencies
##########################################################################
NPT_INCLUDE_MODULES := Core System
include $(NPT_BUILD_INCLUDES)/IncludeModuleDeps.mak

##########################################################################
# exported variables
##########################################################################
NPT_INCLUDES_CPP += -I$(NPT_SOURCE_ROOT)/Neptune

endif
