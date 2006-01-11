#######################################################################
#
#    Common Target Makefile
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

#######################################################################
#    includes
#######################################################################
include $(NPT_ROOT)/Build/Includes/TopLevel.mak
include $(NPT_BUILD_INCLUDES)/BuildConfig.mak
-include ../Local.mak
include ../../../Makefiles/Module.mak
include $(NPT_BUILD_INCLUDES)/Rules.mak
include $(NPT_BUILD_INCLUDES)/AutoDep.mak
