#######################################################################
#
#    Common Makefile Rules
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

##########################################################################
#    pattern rules
##########################################################################

### build .o from .c
%.o: %.c
	$(NPT_COMPILE_C) $(NPT_FLAGS_C) $(NPT_DEFINES_C) $(NPT_INCLUDES_C) $(NPT_OPTIMIZE_C) $(NPT_DEBUG_C) $(NPT_PROFILE_C) $(NPT_WARNINGS_C) $(NPT_LOCAL_FLAGS) -c $< -o $@

### build .o from .cpp
%.o: %.cpp
	$(NPT_COMPILE_CPP) $(NPT_FLAGS_CPP) $(NPT_DEFINES_CPP) $(NPT_INCLUDES_CPP) $(NPT_OPTIMIZE_CPP) $(NPT_DEBUG_CPP) $(NPT_PROFILE_CPP) $(NPT_WARNINGS_CPP) $(NPT_LOCAL_FLAGS) -c $< -o $@

### build .a from .o 
%.a:
	$(NPT_ARCHIVE_O) $@ $(filter %.o %.a %.lib,$^)

### make an executable
NPT_MAKE_EXECUTABLE_COMMAND_CPP = $(NPT_LINK_CPP) $(NPT_OPTIMIZE_CPP) $(NPT_DEBUG_CPP) $(NPT_PROFILE_CPP) $(filter %.o %.a %.lib,$^) $(NPT_LIBRARIES_CPP) -o $@

### make an archive
NPT_MAKE_ARCHIVE_COMMAND = $(NPT_ARCHIVE_A) $@  $(filter %.o %.a %.lib,$^)

### clean
.PHONY: clean
clean:
	@rm -f $(NPT_LOCAL_FILES_TO_CLEAN)

### clean-deps
.PHONY: clean-deps
clean-deps: $(NPT_IMPORTED_MODULE_CLEANS) clean


##########################################################################
#    utils
##########################################################################
NPT_COLOR_SET_1   = "[31;1m"
NPT_COLOR_SET_2   = "[36;1m"
NPT_COLOR_RESET = "[0m"

NPT_MAKE_BANNER_START = @echo $(NPT_COLOR_SET_1)================ making \ $(NPT_COLOR_RESET) $(NPT_COLOR_SET_2) $(XXX_CLIENT)::$@ $(NPT_COLOR_RESET) $(NPT_COLOR_SET_1) ================= $(NPT_COLOR_RESET)

NPT_MAKE_BANNER_END =  @echo $(NPT_COLOR_SET_1)================ done with $(NPT_COLOR_RESET)$(NPT_COLOR_SET_2)$(XXX_CLIENT)::$@ $(NPT_COLOR_RESET) $(NPT_COLOR_SET_1) ================= $(NPT_COLOR_RESET)

NPT_MAKE_BANNER = $(NPT_MAKE_BANNER_START)


NPT_SUB_MAKE = @$(MAKE) $(NPT_MAKE_FLAGS) 
