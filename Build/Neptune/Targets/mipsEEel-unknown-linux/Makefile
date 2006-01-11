#######################################################################
#
#    Bootstrap Makefile
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

### default build config
NPT_BUILD_CONFIG = Debug
export NPT_BUILD_CONFIG

### default target
ifndef MAKECMDGOALS
MAKECMDGOALS = libNeptune.a
endif

$(MAKECMDGOALS): 
	@$(MAKE) -C $(NPT_BUILD_CONFIG) -f ../Common.mak NPT_BUILD_CONFIG=$(NPT_BUILD_CONFIG) $(MAKECMDGOALS)

