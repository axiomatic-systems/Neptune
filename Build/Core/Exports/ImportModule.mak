#######################################################################
#
#    Exports
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

##########################################################################
# exported variables
##########################################################################
NPT_MODULE_LIBRARIES += libNptCore.a

##########################################################################
# module targets
##########################################################################
NPT_CORE_BUILD_DIR = $(NPT_BUILD_ROOT)/Core/Targets/$(NPT_TARGET)

.PHONY: Import-Core
Import-Core:
	$(NPT_MAKE_BANNER_START)
	$(NPT_SUB_MAKE) -C $(NPT_CORE_BUILD_DIR)
	@$(NPT_COPY_IF_NEW) $(NPT_CORE_BUILD_DIR)/$(NPT_BUILD_CONFIG)/libNptCore.a .
	$(NPT_MAKE_BANNER_END)

.PHONY: Clean-Core
Clean-Core:
	$(NPT_MAKE_BANNER_START)
	$(NPT_SUB_MAKE) -C $(NPT_CORE_BUILD_DIR) clean-deps
	$(NPT_MAKE_BANNER_END)
