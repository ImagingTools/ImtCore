macro(ImtCoreGetSdlDeps)

	set(oneValueArgs INPUT OUT_DIR RESULT_VARIABLE)
	set(multiValueArgs MODIFICATORS)
	cmake_parse_arguments(ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	GetSdlGeneratorPath(SDL_GENERATOR_EXE_PATH)
	set(SDL_DEPS_FILE_PATH "${ARG_OUT_DIR}/../__SDL__DependsList.txt")
	set(SDL_ERRORS_FILE_PATH "${ARG_OUT_DIR}/../__SDL__DependsList_errors.txt")

	set(SDL_DEPS_GENERATION_COMMAND "${SDL_GENERATOR_EXE_PATH} -DS ${ARG_INPUT} -O ${ARG_OUT_DIR} ${ARG_MODIFICATORS}")
	execute_process(
		COMMAND
			${SDL_GENERATOR_EXE_PATH} -DS ${ARG_INPUT} -O ${ARG_OUT_DIR} ${ARG_MODIFICATORS}
		OUTPUT_FILE
			${SDL_DEPS_FILE_PATH}
		ERROR_FILE
			${SDL_ERRORS_FILE_PATH}
		RESULT_VARIABLE
			SDL_DEPS_GENERATION_RESULT_CODE
	)

	if(NOT SDL_DEPS_GENERATION_RESULT_CODE EQUAL 0)
		message(WARNING "SDL Can't create dependency list")

		file(STRINGS ${SDL_DEPS_FILE_PATH} ERRORS1_SDL_DEPS_LIST)
		message(WARNING "${ERRORS1_SDL_DEPS_LIST}")

		file(STRINGS ${SDL_ERRORS_FILE_PATH} ERRORS2_SDL_DEPS_LIST)
		message(WARNING "${ERRORS2_SDL_DEPS_LIST}")

		message("EXEC: ${SDL_DEPS_GENERATION_COMMAND}")
		message(FATAL_ERROR "SDL finished unexpected. Error code: [${SDL_DEPS_GENERATION_RESULT_CODE}]")
	endif()

	file(STRINGS ${SDL_DEPS_FILE_PATH} SDL_FOUND_DEPS)

	set(${ARG_RESULT_VARIABLE} ${SDL_FOUND_DEPS})

	# cleanup on success
	file(REMOVE ${SDL_DEPS_FILE_PATH})
	file(REMOVE ${SDL_ERRORS_FILE_PATH})

endmacro()

#! This config file created for SDL-module projects
function (ImtCoreAutoConfigureSdlCpp)
	include (${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ImtSdlConfig.cmake)

	#! \param CUSTOM_TARGETS_DEPS - if enabled, dependencies will NOT be soved, using the SDL generator \warning if enabled, you MUST provide CUSTOM_HEADER_DEP CUSTOM_SOURCE_DEP!
	#! \param CUSTOM_HEADER_DEP - header file dependency				\note this param make sence only if \c CUSTOM_TARGETS_DEPS is enabled
	#! \param CUSTOM_SOURCE_DEP - source file dependency				\note this param make sence only if \c CUSTOM_TARGETS_DEPS is enabled
	set(oneValueArgs CUSTOM_TARGETS_DEPS CUSTOM_HEADER_DEP CUSTOM_SOURCE_DEP SCHEMA_PATH)
	cmake_parse_arguments(ARG "${booleanArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	set(SDL_OUTPUT_DIRECTORY "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL")

	set(CUSTOM_MODIFICATORS)
	list(APPEND CUSTOM_MODIFICATORS "--CPP")
	list(APPEND CUSTOM_MODIFICATORS "--GQL")
	list(APPEND CUSTOM_MODIFICATORS "--use-all-modificators")
	list(APPEND CUSTOM_MODIFICATORS "--auto-join")
	list(APPEND CUSTOM_MODIFICATORS "--generator=CMake")
	list(APPEND CUSTOM_MODIFICATORS "-Psdl")
	list(APPEND CUSTOM_MODIFICATORS "-Bistd::IPolymorphic=istd/IPolymorphic.h")

	if (NOT ARG_CUSTOM_TARGETS_DEPS)
		message (VERBOSE "C: Colletion custom deps")
		ImtCoreGetSdlDeps(
			INPUT
				"${ARG_SCHEMA_PATH}"
			OUT_DIR
				"${SDL_OUTPUT_DIRECTORY}"
			MODIFICATORS
				"${CUSTOM_MODIFICATORS}"
			RESULT_VARIABLE
				FOUND_DEPS)
	else()
		if (NOT ARG_CUSTOM_HEADER_DEP)
			message(FATAL_ERROR "CUSTOM_TARGETS_DEPS is defined, but CUSTOM_HEADER_DEP is not provided!")
		endif()
		if (NOT ARG_CUSTOM_SOURCE_DEP)
			message(FATAL_ERROR "CUSTOM_TARGETS_DEPS is defined, but CUSTOM_SOURCE_DEP is not provided!")
		endif()

		list(APPEND FOUND_DEPS "${SDL_OUTPUT_DIRECTORY}/${ARG_CUSTOM_HEADER_DEP}")
		list(APPEND FOUND_DEPS "${SDL_OUTPUT_DIRECTORY}/${ARG_CUSTOM_SOURCE_DEP}")
	endif()

	message (VERBOSE "C: PATH = ${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL")
	message (VERBOSE "FOUND for ${PROJECT_NAME} DEPS: ${FOUND_DEPS}")
	message (VERBOSE "C: IS CUSTOM: ${ARG_CUSTOM_TARGETS_DEPS}")

	GetSdlGeneratorPath(SDL_GENERATOR_EXE_PATH)

	add_custom_command(
		OUTPUT
			${FOUND_DEPS}
		COMMAND
			${SDL_GENERATOR_EXE_PATH}
		ARGS
			-GS ${ARG_SCHEMA_PATH} -O "${SDL_OUTPUT_DIRECTORY}" ${CUSTOM_MODIFICATORS}
		DEPENDS
			${SDL_GENERATOR_EXE_PATH} ${ARG_SCHEMA_PATH}
		COMMENT
			"[CPP:SDL::${PROJECT_NAME}] Creating classes for schema ${ARG_SCHEMA_PATH}"
		VERBATIM)

	target_sources(${PROJECT_NAME} PRIVATE ${FOUND_DEPS})

endfunction()

function (ImtCoreAutoConfigureSdlQml)
	include (${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ImtSdlConfig.cmake)

	#! \param CUSTOM_TARGETS_DEPS - if enabled, dependencies will NOT be soved, using the SDL generator \warning if enabled, you MUST provide CUSTOM_QML_DEP!
	#! \param CUSTOM_QML_RES_DEP - QML resource (.qrc) file dependency	\note this param make sence only if \c CUSTOM_TARGETS_DEPS is enabled
	set(oneValueArgs CUSTOM_TARGETS_DEPS CUSTOM_QML_RES_DEP SCHEMA_PATH)
	cmake_parse_arguments(ARG "${booleanArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	set(SDL_OUTPUT_DIRECTORY "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL")

	set(MODIFICATORS)
	list(APPEND MODIFICATORS "--QML")
	list(APPEND MODIFICATORS "--use-all-modificators")
	list(APPEND MODIFICATORS "--generator=CMake")
	list(APPEND MODIFICATORS "--auto-join")

	if (NOT ARG_CUSTOM_TARGETS_DEPS)
		ImtCoreGetSdlDeps(
			INPUT
				"${ARG_SCHEMA_PATH}"
			OUT_DIR
				"${SDL_OUTPUT_DIRECTORY}"
			MODIFICATORS
				"${MODIFICATORS}"
			RESULT_VARIABLE
				FOUND_DEPS)
	else ()
		if (NOT ARG_CUSTOM_QML_RES_DEP)
			message(FATAL_ERROR "CUSTOM_TARGETS_DEPS is defined, but CUSTOM_QML_RES_DEP is not provided!")
		endif()
		list(APPEND FOUND_DEPS "${SDL_OUTPUT_DIRECTORY}/${ARG_CUSTOM_QML_RES_DEP}")
	endif()

	message(VERBOSE "QML DEP FOUND for ${PROJECT_NAME} DEPS: ${FOUND_DEPS}")

	GetSdlGeneratorPath(SDL_GENERATOR_EXE_PATH)
	add_custom_command(
		OUTPUT
			${FOUND_DEPS}
		COMMAND
			${SDL_GENERATOR_EXE_PATH}
		ARGS
			-GS ${ARG_SCHEMA_PATH} -O "${SDL_OUTPUT_DIRECTORY}" ${MODIFICATORS}
		DEPENDS
			${SDL_GENERATOR_EXE_PATH} ${ARG_SCHEMA_PATH}
		COMMENT
			"[QML:SDL::${PROJECT_NAME}] Creating resources for schema ${ARG_SCHEMA_PATH}"
		VERBATIM)

	ImtFutureResourceWrap (
		${PROJECT_NAME}
		"${FOUND_DEPS}"
	)
endfunction()


# CPP+QML+GQL WARNING this function enables ALL modificators

function (ImtCoreAutoConfigureSdlCppQml)
	#! \param CUSTOM_TARGETS_DEPS - if enabled, dependencies will NOT be soved, using the SDL generator \warning if enabled, you MUST provide CUSTOM_HEADER_DEP CUSTOM_SOURCE_DEP CUSTOM_QML_DEP!
	#! \param CUSTOM_HEADER_DEP - header file dependency				\note this param make sence only if \c CUSTOM_TARGETS_DEPS is enabled
	#! \param CUSTOM_SOURCE_DEP - source file dependency				\note this param make sence only if \c CUSTOM_TARGETS_DEPS is enabled
	#! \param CUSTOM_QML_RES_DEP - QML resource (.qrc) file dependency	\note this param make sence only if \c CUSTOM_TARGETS_DEPS is enabled
	# set(booleanArgs CUSTOM_TARGETS_DEPS)
	set(oneValueArgs CUSTOM_TARGETS_DEPS CUSTOM_HEADER_DEP CUSTOM_SOURCE_DEP CUSTOM_QML_RES_DEP)
	set(multiValueArgs SDL_SOURCES)
	cmake_parse_arguments(ARG "${booleanArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	message (VERBOSE "${PROJECT_NAME} CFG: IS CUSTOM: ${ARG_CUSTOM_TARGETS_DEPS}")
	foreach(SDL_FILE_PATH IN LISTS ARG_SDL_SOURCES)
		message(VERBOSE "Additing SDL source '${SDL_FILE_PATH}' for ${PROJECT_NAME}")

		ImtCoreAutoConfigureSdlCpp(SCHEMA_PATH "${SDL_FILE_PATH}" CUSTOM_TARGETS_DEPS "${ARG_CUSTOM_TARGETS_DEPS}" CUSTOM_HEADER_DEP "${ARG_CUSTOM_HEADER_DEP}" CUSTOM_SOURCE_DEP "${ARG_CUSTOM_SOURCE_DEP}")
		ImtCoreAutoConfigureSdlQml(SCHEMA_PATH "${SDL_FILE_PATH}" CUSTOM_TARGETS_DEPS "${ARG_CUSTOM_TARGETS_DEPS}" CUSTOM_QML_RES_DEP "${ARG_CUSTOM_QML_RES_DEP}")
	endforeach()

endfunction()
