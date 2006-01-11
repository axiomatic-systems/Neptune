#######################################################################
#
#    Module Dependencies Processing
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################
ifneq ($(NPT_INCLUDE_MODULES),)
include $(foreach module,$(NPT_INCLUDE_MODULES),$(NPT_BUILD_MODULES)/Include$(module).mak)
NPT_INCLUDE_MODULES :=
endif

