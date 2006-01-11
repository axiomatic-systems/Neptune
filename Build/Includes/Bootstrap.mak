#######################################################################
#
#    Bootstrap Makefile
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

ifndef NPT_BUILD_CONFIG
NPT_BUILD_CONFIG=Debug
endif
 
export NPT_ROOT
export NPT_TARGET
export NPT_BUILD_CONFIG
 
ifndef MAKECMDGOALS
MAKECMDGOALS = $(NPT_DEFAULT_GOAL)
endif
 
$(MAKECMDGOALS):
	@[ -d $(NPT_BUILD_CONFIG) ] || mkdir $(NPT_BUILD_CONFIG)
	@$(MAKE) --no-print-directory -C $(NPT_BUILD_CONFIG) -f $(NPT_ROOT)/Build/Includes/Common.mak $(MAKECMDGOALS)
