#######################################################################
#
#    Generic Makefiles Rules
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

#######################################################################
#  check configuration variables
#######################################################################
ifndef NPT_TARGET
ifdef MAKECMDGOALS
$(error "NPT_TARGET variable is not set")
endif
endif

ifndef NPT_BUILD_CONFIG
# default build configuration
NPT_BUILD_CONFIG=Debug
endif

#######################################################################
#  target templates
#######################################################################
NPT_ALL_BUILDS = $(NPT_SUBDIR_BUILDS) $(NPT_SUBTARGET_BUILDS)
All: $(NPT_ALL_BUILDS)
Clean-All: $(foreach goal,$(NPT_ALL_BUILDS),Clean-$(goal))

NPT_SUBDIR_CLEANS = $(foreach dir,$(NPT_SUBDIR_BUILDS),Clean-$(dir))
NPT_SUBTARGET_CLEANS = $(foreach dir,$(NPT_SUBTARGET_BUILDS),Clean-$(dir))

.PHONY: $(NPT_SUBDIR_BUILDS)
$(NPT_SUBDIR_BUILDS): 
	$(NPT_MAKE_BANNER_START)
	$(NPT_SUB_MAKE) -C $@ All
	$(NPT_MAKE_BANNER_END)

.PHONY: $(NPT_SUBTARGET_BUILDS)
$(NPT_SUBTARGET_BUILDS): 
	$(NPT_MAKE_BANNER_START)
	$(NPT_SUB_MAKE) -C $@/Targets/$(NPT_TARGET)
	$(NPT_MAKE_BANNER_END)

.PHONY: $(NPT_SUBDIR_CLEANS)
$(NPT_SUBDIR_CLEANS): 
	$(NPT_MAKE_BANNER_START)
	$(NPT_SUB_MAKE) -C $(subst Clean-,,$@) Clean-All
	$(NPT_MAKE_BANNER_END)

.PHONY: $(NPT_SUBTARGET_CLEANS)
$(NPT_SUBTARGET_CLEANS): 
	$(NPT_MAKE_BANNER_START)
	$(NPT_SUB_MAKE) -C $(subst Clean-,,$@)/Targets/$(NPT_TARGET) clean-deps
	$(NPT_MAKE_BANNER_END)
