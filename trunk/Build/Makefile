#######################################################################
#
#    Master Makefile
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

#######################################################################
#  targets    
#######################################################################
all:
	@echo "Master Makefile, choose one of:"
	@echo "-------------------------------"
	@echo ""
	@echo "SDK:         build all libraries and header files"
	@echo "Clean-SDK:   clean all libraries and header files"
	@echo ""
	@echo "Apps:        build all applications"
	@echo "Clean-Apps:  clean all applications"
	@echo ""
	@echo "Tests:       build all tests"
	@echo "Clean-Tests: clean all tests"
	@echo ""
	@echo "All:         build everything"
	@echo "Clean-All:   clean everything"
	@echo 
	@echo "Supported targets:"
	@echo $(subst Includes/Targets/,$(NL),$(wildcard Includes/Targets/*-*-*))

### aliases
SDK: Neptune
Clean-SDK: Clean-Neptune

### subdirs
NPT_SUBDIR_BUILDS = Apps Tests
NPT_SUBTARGET_BUILDS = Neptune

#######################################################################
#  includes
#######################################################################
include Includes/Generic.mak
include Includes/Rules.mak
include Includes/Exports.mak

