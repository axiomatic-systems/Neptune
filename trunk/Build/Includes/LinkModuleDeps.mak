#######################################################################
#
#    Module Dependencies Processing
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################
ifneq ($(NPT_LINK_MODULES),)
include $(foreach module,$(NPT_LINK_MODULES),$(NPT_BUILD_MODULES)/Link$(module).mak)
NPT_LINK_MODULES :=
endif

