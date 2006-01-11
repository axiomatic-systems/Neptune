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
NPT_MODULE_LIBRARIES += libNptSystem.a

##########################################################################
# module targets
##########################################################################
NPT_SYSTEM_BUILD_DIR = $(NPT_BUILD_ROOT)/System/Targets/$(NPT_TARGET)

.PHONY: Import-System
Import-System:
	$(NPT_MAKE_BANNER_START)
	$(NPT_SUB_MAKE) -C $(NPT_SYSTEM_BUILD_DIR)
	@$(NPT_COPY_IF_NEW) $(NPT_SYSTEM_BUILD_DIR)/$(NPT_BUILD_CONFIG)/libNptSystem.a .
	$(NPT_MAKE_BANNER_END)

.PHONY: Clean-System
Clean-System:
	$(NPT_MAKE_BANNER_START)
	$(NPT_SUB_MAKE) -C $(NPT_SYSTEM_BUILD_DIR) clean-deps
	$(NPT_MAKE_BANNER_END)
