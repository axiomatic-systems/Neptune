#######################################################################
#
#    Include file for Neptune Clients
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################
ifndef XXX_CLIENT
$(error Variable XXX_CLIENT is not defined)
endif

ifndef NEPTUNE_ROOT
$(error Variable NEPTUNE_ROOT is not defined)
endif

ifeq ($(NPT_NEPTUNE_IMPORTED),)
NPT_NEPTUNE_IMPORTED := yes

##########################################################################
# client variables
##########################################################################
$(XXX_CLIENT)_MODULE_LIBRARIES += libNeptune.a
#$(XXX_CLIENT)_LIBRARIES_CPP    += -lpthread

##########################################################################
# dependencies
##########################################################################
include $(NEPTUNE_ROOT)/Build/Includes/IncludeNeptune.mak

##########################################################################
# module variables
##########################################################################
NPT_BUILD_DIR = $(NEPTUNE_ROOT)/Build/Neptune/Targets/$($(XXX_CLIENT)_TARGET)

##########################################################################
# targets
##########################################################################
.PHONY: Import-Neptune
Import-Neptune:
	$($(XXX_CLIENT)_MAKE_BANNER_START)
	$($(XXX_CLIENT)_SUB_MAKE) -C $(NPT_BUILD_DIR) NPT_BUILD_CONFIG=$($(XXX_CLIENT)_BUILD_CONFIG)
	@$($(XXX_CLIENT)_COPY_IF_NEW) $(NPT_BUILD_DIR)/$($(XXX_CLIENT)_BUILD_CONFIG)/libNeptune.a .
	$($(XXX_CLIENT)_MAKE_BANNER_END)

.PHONY: Clean-Neptune
Clean-Neptune:
	$($(XXX_CLIENT)_MAKE_BANNER_START)
	$($(XXX_CLIENT)_SUB_MAKE) -C $(NPT_BUILD_DIR) NPT_BUILD_CONFIG=$($(XXX_CLIENT)_BUILD_CONFIG) clean-deps 
	$($(XXX_CLIENT)_MAKE_BANNER_END)

endif
