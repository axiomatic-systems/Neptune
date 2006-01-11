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

ifeq ($(NPT_NEPTUNE_INCLUDED),)
NPT_NEPTUNE_INCLUDED := yes

##########################################################################
# client variables
##########################################################################
$(XXX_CLIENT)_INCLUDES_CPP += -I$(NEPTUNE_ROOT)/Source/Neptune -I$(NEPTUNE_ROOT)/Source/Core -I$(NEPTUNE_ROOT)/Source/Config

endif
