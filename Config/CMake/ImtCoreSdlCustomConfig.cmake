macro(ImtCoreGetSdlDeps)

	set(booleanArgs GET_SCHEMA_DEPS)
	set(oneValueArgs INPUT OUT_DIR RESULT_VARIABLE)
	set(multiValueArgs MODIFICATORS)
	cmake_parse_arguments(ARG "${booleanArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	GetSdlGeneratorPath(SDL_GENERATOR_EXE_PATH)

	if (ARG_GET_SCHEMA_DEPS)
		set(SDL_DEPS_GENERATION_COMMAND ${SDL_GENERATOR_EXE_PATH} -S ${ARG_INPUT} --schema-dependencies)
		set(SDL_ERRORS_FILE_PATH "${ARG_OUT_DIR}/__SDL_SCHEMA__DependsList_errors.txt")
	else()
		set(SDL_DEPS_GENERATION_COMMAND ${SDL_GENERATOR_EXE_PATH} -DS ${ARG_INPUT} -O ${ARG_OUT_DIR} ${ARG_MODIFICATORS})
		set(SDL_ERRORS_FILE_PATH "${ARG_OUT_DIR}/__SDL__DependsList_errors.txt")
	endif()

	message(STATUS "EXEC: ${SDL_DEPS_GENERATION_COMMAND} --generator=CMake-pipe")

	execute_process(
		COMMAND
			${SDL_DEPS_GENERATION_COMMAND} --generator=CMake-pipe
		ERROR_FILE
			${SDL_ERRORS_FILE_PATH}
		RESULT_VARIABLE
			SDL_DEPS_GENERATION_RESULT_CODE
		OUTPUT_VARIABLE
			SDL_DEPS_VAR
	)

	# remove empty elements
	list(JOIN SDL_DEPS_VAR "|" SDL_DEPS_VAR)
	string(REGEX REPLACE "\\|$" "" SDL_DEPS_VAR "${SDL_DEPS_VAR}")
	string(REGEX REPLACE "\\|\\|" "|" SDL_DEPS_VAR "${SDL_DEPS_VAR}")
	string(REPLACE "|" ";" SDL_DEPS_VAR "${SDL_DEPS_VAR}")

	if(NOT SDL_DEPS_GENERATION_RESULT_CODE EQUAL 0)
		message(WARNING "SDL Can't create dependency list")
		message(WARNING "${SDL_DEPS_VAR}")

		file(STRINGS ${SDL_ERRORS_FILE_PATH} ERRORS2_SDL_DEPS_LIST)
		message(WARNING "${ERRORS2_SDL_DEPS_LIST}")

		message(STAUTS"EXEC: ${SDL_DEPS_GENERATION_COMMAND}")
		message(FATAL_ERROR "SDL finished unexpected. Error code: [${SDL_DEPS_GENERATION_RESULT_CODE}]")
	endif()

	set(${ARG_RESULT_VARIABLE} ${SDL_DEPS_VAR})

	# cleanup on success
	file(REMOVE ${SDL_ERRORS_FILE_PATH})

endmacro()


#! \param CUSTOM_TARGETS_DEPS - if enabled, dependencies will NOT be soved, using the SDL generator \warning if enabled, you MUST provide CUSTOM_HEADER_DEP CUSTOM_SOURCE_DEP!
#! \param CUSTOM_SOURCE_DEP - source file dependency				\note this param make sence only if \c CUSTOM_TARGETS_DEPS is enabled
function (ImtCoreAutoConfigureSdlCpp)
	include (${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ImtSdlConfig.cmake)

	set(oneValueArgs
				SCHEMA_PATH
				VERSION
				SOURCE_NAME)
	cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})

	set(SDL_OUTPUT_DIRECTORY "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL/${ARG_VERSION}/CPP")

	set(CUSTOM_MODIFICATORS)
	list(APPEND CUSTOM_MODIFICATORS "--CPP")
	list(APPEND CUSTOM_MODIFICATORS "--GQL")
	list(APPEND CUSTOM_MODIFICATORS "--use-all-modificators")
	list(APPEND CUSTOM_MODIFICATORS "-Psdl")
	list(APPEND CUSTOM_MODIFICATORS "-Bistd::IPolymorphic=istd/IPolymorphic.h")
	list(APPEND CUSTOM_MODIFICATORS "-JCPP=${SDL_OUTPUT_DIRECTORY}/${ARG_SOURCE_NAME}.cpp")
	list(APPEND CUSTOM_MODIFICATORS "-JH=${SDL_OUTPUT_DIRECTORY}/${ARG_SOURCE_NAME}.h")

	ImtCoreGetSdlDeps(
		INPUT
			"${ARG_SCHEMA_PATH}"
		OUT_DIR
			"${SDL_OUTPUT_DIRECTORY}"
		MODIFICATORS
			"${CUSTOM_MODIFICATORS}"
		RESULT_VARIABLE
			FOUND_DEPS)

	ImtCoreGetSdlDeps(GET_SCHEMA_DEPS
		INPUT
			"${ARG_SCHEMA_PATH}"
		OUT_DIR
			"${SDL_OUTPUT_DIRECTORY}"
		MODIFICATORS
			"${CUSTOM_MODIFICATORS}"
		RESULT_VARIABLE
			FOUND_SCHEMA_DEPS)

	message(VERBOSE "CPP: FOUND for ${PROJECT_NAME} DEPS: ${FOUND_DEPS} SCHEMAS: ${FOUND_SCHEMA_DEPS}")

	GetSdlGeneratorPath(SDL_GENERATOR_EXE_PATH)

	add_custom_command(
		OUTPUT
			${FOUND_DEPS}
		COMMAND
			${SDL_GENERATOR_EXE_PATH}
		ARGS
			-GS ${ARG_SCHEMA_PATH} -O "${SDL_OUTPUT_DIRECTORY}" ${CUSTOM_MODIFICATORS}
		DEPENDS
			${SDL_GENERATOR_EXE_PATH} ${ARG_SCHEMA_PATH} "${FOUND_SCHEMA_DEPS}"
		COMMENT
			"[CPP:SDL::${PROJECT_NAME}] Creating classes"
		VERBATIM)

	target_sources(${PROJECT_NAME} PRIVATE ${FOUND_DEPS})

endfunction()


function (ImtCoreAutoConfigureSdlQml)
	include (${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ImtSdlConfig.cmake)

	set(oneValueArgs SCHEMA_PATH VERSION QML_NAME)
	cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})

	# "${ARG_QML_NAME}"
	set(SDL_OUTPUT_DIRECTORY "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL/${ARG_VERSION}/QML/${ARG_QML_NAME}")

	set(MODIFICATORS)
	list(APPEND MODIFICATORS "--QML")
	list(APPEND MODIFICATORS "--use-all-modificators")
	list(APPEND MODIFICATORS "-N=${ARG_QML_NAME}")

	ImtCoreGetSdlDeps(
		INPUT
			"${ARG_SCHEMA_PATH}"
		OUT_DIR
			"${SDL_OUTPUT_DIRECTORY}"
		MODIFICATORS
			"${MODIFICATORS}"
		RESULT_VARIABLE
			FOUND_DEPS)


	ImtCoreGetSdlDeps(GET_SCHEMA_DEPS
		INPUT
			"${ARG_SCHEMA_PATH}"
		OUT_DIR
			"${SDL_OUTPUT_DIRECTORY}"
		MODIFICATORS
			"${CUSTOM_MODIFICATORS}"
		RESULT_VARIABLE
			FOUND_SCHEMA_DEPS)

	message(VERBOSE "QML DEP FOUND for ${PROJECT_NAME} DEPS: ${FOUND_DEPS} SCHEMAS: ${FOUND_SCHEMA_DEPS}")

	GetSdlGeneratorPath(SDL_GENERATOR_EXE_PATH)
	add_custom_command(
		OUTPUT
			${FOUND_DEPS}
		COMMAND
			${SDL_GENERATOR_EXE_PATH}
		ARGS
			-GS ${ARG_SCHEMA_PATH} -O "${SDL_OUTPUT_DIRECTORY}" ${MODIFICATORS}
		DEPENDS
			${SDL_GENERATOR_EXE_PATH} ${ARG_SCHEMA_PATH} "${FOUND_SCHEMA_DEPS}"
		COMMENT
			"[QML:SDL::${PROJECT_NAME}] Creating resources"
		VERBATIM)

	ImtFutureResourceWrap (
		${PROJECT_NAME}
		"${FOUND_DEPS}"
	)
endfunction()


# CPP+QML+GQL WARNING this function enables ALL modificators

#! \param SCHEMA_PATH	- The absolute path to the schema file to be compiled.
#! \param VERSION		- Defines the schema's version (also part of the OUTPUT directory path)
#! \param SOURCE_NAME	- The output source filename, should be provided without an extension, and the function automatically adds the appropriate extension(.cpp).
#! \param QML_NAME		- The QML module name. Also part of the OUTPUT directory path and filename of QRC file
function (ImtCoreCustomConfigureSdlCppQml)
	set(oneValueArgs
		SCHEMA_PATH
		VERSION
		SOURCE_NAME
		QML_NAME)
	cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})

	if (ARG_SOURCE_NAME)
		ImtCoreAutoConfigureSdlCpp(
			SCHEMA_PATH "${ARG_SCHEMA_PATH}"
			VERSION "${ARG_VERSION}"
			SOURCE_NAME	"${ARG_SOURCE_NAME}")
	endif()

	if (ARG_QML_NAME)
		message(STATUS "Additing SDL for QML compile source '${ARG_SCHEMA_PATH}' for ${PROJECT_NAME}")
		ImtCoreAutoConfigureSdlQml(SCHEMA_PATH "${ARG_SCHEMA_PATH}" VERSION "${ARG_VERSION}" QML_NAME "${ARG_QML_NAME}")
	endif()

endfunction()
