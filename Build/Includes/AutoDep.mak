#######################################################################
#
#    Build Definitions for Automatic Dependencies
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

##########################################################################
# rules
##########################################################################

### autodependency for .cpp files
ifneq ($(NPT_AUTODEP_STDOUT),)
%.d: %.cpp
	$(NPT_AUTODEP_CPP) $(NPT_DEFINES_CPP) $(NPT_INCLUDES_CPP) $< > $@
else
%.d: %.cpp
	$(NPT_AUTODEP_CPP) $(NPT_DEFINES_CPP) $(NPT_INCLUDES_CPP) $< -o $@
endif

### autodependency for .c files
ifneq ($(NPT_AUTODEP_STDOUT),)
%.d: %.c
	$(NPT_AUTODEP_C) $(NPT_DEFINES_C) $(NPT_INCLUDES_C) $< > $@
else
%.d: %.c
	$(NPT_AUTODEP_C) $(NPT_DEFINES_C) $(NPT_INCLUDES_C) $< -o $@
endif

##########################################################################
# auto dependencies
##########################################################################
NPT_MODULE_DEPENDENCIES := $(patsubst %.c,%.d,$(NPT_MODULE_SOURCES))
NPT_MODULE_DEPENDENCIES := $(patsubst %.cpp,%.d,$(NPT_MODULE_SOURCES))

ifneq ($(MAKECMDGOALS), clean)
ifneq ($(MAKECMDGOALS), clean-deps)
ifdef NPT_MODULE_DEPENDENCIES
include $(NPT_MODULE_DEPENDENCIES)
endif
ifdef NPT_MODULE_LOCAL_RULES
include $(NPT_MODULE_LOCAL_RULES)
endif
endif
endif
