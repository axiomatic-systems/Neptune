#######################################################################
#
#    Module Exports
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

##########################################################################
# includes
##########################################################################
include $(NPT_BUILD_ROOT)/Neptune/Exports/IncludeModule.mak

##########################################################################
# libraries
##########################################################################
NPT_MODULE_LIBRARIES += libNeptune.a

##########################################################################
# module variables
##########################################################################
NPT_BUILD_DIR = $(NPT_ROOT)/Build/Neptune/Targets/$(NPT_TARGET)

##########################################################################
# targets
##########################################################################
.PHONY: Import-Neptune
Import-Neptune:
	$(NPT_MAKE_BANNER_START)
	$(NPT_SUB_MAKE) -C $(NPT_BUILD_DIR)
	@$(NPT_COPY_IF_NEW) $(NPT_BUILD_DIR)/$(NPT_BUILD_CONFIG)/libNeptune.a .
	$(NPT_MAKE_BANNER_END)

.PHONY: Clean-Neptune
Clean-Neptune:
	$(NPT_MAKE_BANNER_START)
	$(NPT_SUB_MAKE) -C $(NPT_BUILD_DIR) clean-deps
	$(NPT_MAKE_BANNER_END)


