
include(${ACFDIR}/Config/CMake/GeneralConfig.cmake)

#----------------------------------------------------------- The DESIGN TOKEN CREATOR config
# Setting the executable file name for specific OS
set(COMPILER_DIR ${CMAKE_BUILD_TYPE}${TARGETNAME})
if (win32)
	set(DESIGN_TOKEN_CREATOR_EXE "DesignTokenCreatorExe.exe")
else()
	set(DESIGN_TOKEN_CREATOR_EXE "DesignTokenCreatorExe")
endif()

#if(APPLE)
#	set(DESIGN_TOKEN_CREATOR_EXE "DesignTokenCreatorExe.app/Contents/MacOS/DesignTokenCreatorExe")
#endif()

#  Setting the executable absolutely file path for specific OS
set(DESIGN_TOKEN_CREATOR_EXE_BIN "${CMAKE_CURRENT_SOURCE_DIR}/../../../Bin/${COMPILER_DIR}/${DESIGN_TOKEN_CREATOR_EXE}")

set(DESIGN_TOKEN_CREATOR_COMMAND_PARAMS -I -F ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE} -ID ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_INPUT_DIR} -OD ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR})

add_custom_command(
	OUTPUT
		${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${PROJECT_NAME}DesignToekn.stamp
	COMMAND
		${DESIGN_TOKEN_CREATOR_EXE_BIN}
	ARGS
		${DESIGN_TOKEN_CREATOR_COMMAND_PARAMS}
	COMMAND
		${CMAKE_COMMAND} -E touch ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${PROJECT_NAME}DesignToekn.stamp
	COMMENT
		"Creating images for ${PROJECT_NAME}"
	VERBATIM
	)

add_custom_target(
	designToekn ALL
	DEPENDS ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${PROJECT_NAME}DesignToekn.stamp
	)





