#######################################################################
#
#    Module Dependencies Processing
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################
ifneq ($(NPT_IMPORT_MODULES),)
NPT_IMPORTED_MODULE_LIBS  := $(foreach module,$(NPT_IMPORT_MODULES),Import-$(module))
NPT_IMPORTED_MODULE_CLEANS := $(foreach module,$(NPT_IMPORT_MODULES),Clean-$(module))
include $(foreach module,$(NPT_IMPORT_MODULES),$(NPT_BUILD_MODULES)/Import$(module).mak)
NPT_IMPORT_MODULES :=
endif
