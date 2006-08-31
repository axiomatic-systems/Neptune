#######################################################################
#
#    Module Makefile
#
#    (c) 2002-2003 Gilles Boccon-Gibod
#    Author: Gilles Boccon-Gibod (bok@bok.net)
#
#######################################################################

#######################################################################
# module dependencies
#######################################################################
NPT_IMPORT_MODULES = Core System
include $(NPT_BUILD_INCLUDES)/ImportModuleDeps.mak

#######################################################################
# clean
#######################################################################
NPT_LOCAL_FILES_TO_CLEAN = *.d *.o *.a SDK/*.* SDK/*/*.*

#######################################################################
# targets
#######################################################################
NPT_SDK_DIR         = SDK
NPT_SDK_LIB_DIR     = SDK/lib
NPT_SDK_INCLUDE_DIR = SDK/include

.PHONY: dirs
dirs:
	-@mkdir -p $(NPT_SDK_DIR)
	-@mkdir -p $(NPT_SDK_LIB_DIR)
	-@mkdir -p $(NPT_SDK_INCLUDE_DIR)

libNeptune.a: $(NPT_IMPORTED_MODULE_LIBS) $(NPT_MODULE_OBJECTS) $(NPT_MODULE_LIBRARIES)
	$(NPT_MAKE_BANNER_START)
	$(NPT_MAKE_ARCHIVE_COMMAND)
	$(NPT_MAKE_BANNER_END)

.PHONY: SDK
SDK: dirs libNeptune.a
	$(NPT_MAKE_BANNER_START)
	@cp libNeptune.a $(NPT_SDK_LIB_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptStreams.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptCommon.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptConstants.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptDefs.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptDebug.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptFile.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptInterfaces.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptList.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptArray.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptMap.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptStack.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptBase64.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptMessaging.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptQueue.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptResults.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptRingBuffer.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptSelectableMessageQueue.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptSimpleMessageQueue.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptSockets.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptNetwork.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptStrings.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptSystem.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptThreads.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptTime.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptTypes.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptXml.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptUri.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptHttp.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptDataBuffer.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptBufferedStreams.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptUtils.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptBufferedStreams.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptNetwork.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Core/NptReferences.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Config/NptConfig.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Neptune/NptVersion.h $(NPT_SDK_INCLUDE_DIR)
	@cp $(NPT_SOURCE_ROOT)/Neptune/Neptune.h $(NPT_SDK_INCLUDE_DIR)
	$(NPT_MAKE_BANNER_END)
