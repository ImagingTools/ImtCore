include (${CMAKE_CURRENT_LIST_DIR}/ImtSdlConfig.cmake)

macro(ImtCoreGetSdlDeps)

	set(booleanArgs GET_SCHEMA_DEPS)
	set(oneValueArgs INPUT OUT_DIR RESULT_VARIABLE)
	set(multiValueArgs MODIFICATORS)
	cmake_parse_arguments(ARG "${booleanArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	GetSdlGeneratorPath(SDL_GENERATOR_EXE_PATH)

	if (ARG_GET_SCHEMA_DEPS)
		set(SDL_DEPS_GENERATION_COMMAND ${SDL_GENERATOR_EXE_PATH} -S ${ARG_INPUT} --schema-dependencies --auto-link=2)

		list(LENGTH GLOBAL_SDL_SCHEMA_SEARCH_PATHS SDL_SCHEMA_PATHS_COUNT)
		if (SDL_SCHEMA_PATHS_COUNT GREATER 0 )
			foreach(SDL_SEARCH_PATH ${GLOBAL_SDL_SCHEMA_SEARCH_PATHS})
				list(APPEND SDL_DEPS_GENERATION_COMMAND "-I${SDL_SEARCH_PATH}")
			endforeach()
		endif()

		set(SDL_ERRORS_FILE_PATH "${ARG_OUT_DIR}/__SDL_SCHEMA__DependsList_errors.txt")
	else()
		set(SDL_DEPS_GENERATION_COMMAND ${SDL_GENERATOR_EXE_PATH} -DS ${ARG_INPUT} -O ${ARG_OUT_DIR} ${ARG_MODIFICATORS})
		set(SDL_ERRORS_FILE_PATH "${ARG_OUT_DIR}/__SDL__DependsList_errors.txt")
	endif()

	file(MAKE_DIRECTORY ${ARG_OUT_DIR})

	message(VERBOSE "EXEC: ${SDL_DEPS_GENERATION_COMMAND} --generator=CMake-pipe")

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
	# join a|b|c
	list(JOIN SDL_DEPS_VAR "|" SDL_DEPS_VAR)
	# remove a||c -> a|c
	string(REGEX REPLACE "\\|\\|" "|" SDL_DEPS_VAR "${SDL_DEPS_VAR}")
	# remove last empty a|c| -> a|c
	string(REGEX REPLACE "\\|$" "" SDL_DEPS_VAR "${SDL_DEPS_VAR}")
	# transform into a list
	string(REPLACE "|" ";" SDL_DEPS_VAR "${SDL_DEPS_VAR}")

	if(NOT SDL_DEPS_GENERATION_RESULT_CODE EQUAL 0)
		message(WARNING "SDL Can't create dependency list")
		message(WARNING "${SDL_DEPS_VAR}")

		file(STRINGS ${SDL_ERRORS_FILE_PATH} ERRORS2_SDL_DEPS_LIST)
		message(WARNING "${ERRORS2_SDL_DEPS_LIST}")

		message(STATUS "EXEC: ${SDL_DEPS_GENERATION_COMMAND}")
		message(FATAL_ERROR "SDL finished unexpected. Error code: [${SDL_DEPS_GENERATION_RESULT_CODE}]")
	endif()

	set(${ARG_RESULT_VARIABLE} ${SDL_DEPS_VAR})

	# cleanup on success
	file(REMOVE ${SDL_ERRORS_FILE_PATH})

endmacro()

# TODO Rename it to ImtCoreAddSdlHeaderIncludePath
function(ImtCoreAddSdlSearchPath ARG_SDL_PATH)
	if (NOT GLOBAL_SDL_SEARCH_PATHS)
		set(GLOBAL_SDL_SEARCH_PATHS ${ARG_SDL_PATH}
			CACHE
			STRING "List of directories to search for generated header files, used by SDL code generator"
			FORCE
		)

	else()
		list(APPEND GLOBAL_SDL_SEARCH_PATHS ${ARG_SDL_PATH})
		list(REMOVE_DUPLICATES GLOBAL_SDL_SEARCH_PATHS)
		set(GLOBAL_SDL_SEARCH_PATHS ${GLOBAL_SDL_SEARCH_PATHS}
			CACHE
			STRING "List of directories to search for generated header files, used by SDL code generator"
			FORCE
		)
	endif()
endfunction()

function(ImtCoreAddSchemaSearchPath ARG_SDL_PATH)
	if (NOT GLOBAL_SDL_SCHEMA_SEARCH_PATHS)
		set(GLOBAL_SDL_SCHEMA_SEARCH_PATHS ${ARG_SDL_PATH}
			CACHE
			STRING "List of directories to search for schema files, used by SDL code generator"
			FORCE
		)

	else()
		list(APPEND GLOBAL_SDL_SCHEMA_SEARCH_PATHS ${ARG_SDL_PATH})
		list(REMOVE_DUPLICATES GLOBAL_SDL_SCHEMA_SEARCH_PATHS)
		set(GLOBAL_SDL_SCHEMA_SEARCH_PATHS ${GLOBAL_SDL_SCHEMA_SEARCH_PATHS}
			CACHE
			STRING "List of directories to search for schema files, used by SDL code generator"
			FORCE
		)
	endif()
endfunction()


macro (ImtCoreCustomConfigureSdlCpp)
	set(oneValueArgs
				SCHEMA_PATH
				VERSION
				SOURCE_NAME
				EXTRA_DEPS
				FOUND_DEPS
				CUSTOM_OUTPUT_ROOT_DIR_PATH)
	cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})

	set (SDL_OUTPUT_ROOT_DIRECTORY "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL")
	if (ARG_CUSTOM_OUTPUT_ROOT_DIR_PATH)
		set (SDL_OUTPUT_ROOT_DIRECTORY "${ARG_CUSTOM_OUTPUT_ROOT_DIR_PATH}")
	endif()
	set(SDL_OUTPUT_DIRECTORY "${SDL_OUTPUT_ROOT_DIRECTORY}")
	set(SDL_CPP_OUTPUT_DIRECTORY "${SDL_OUTPUT_ROOT_DIRECTORY}/${ARG_VERSION}/CPP")

	set(CUSTOM_MODIFICATORS)
	list(APPEND CUSTOM_MODIFICATORS "--GG=2") ##< Compile, using a new version of code generator.
	list(APPEND CUSTOM_MODIFICATORS "--CPP")
	list(APPEND CUSTOM_MODIFICATORS "--GQL")
	list(APPEND CUSTOM_MODIFICATORS "--use-all-modificators")
	list(APPEND CUSTOM_MODIFICATORS "-Psdl")
	list(APPEND CUSTOM_MODIFICATORS "-Bistd::IPolymorphic=istd/IPolymorphic.h")
	list(APPEND CUSTOM_MODIFICATORS "--auto-link=2") ##< Compile the schema provided exclusively.

	if (ARG_SOURCE_NAME)
		list(APPEND CUSTOM_MODIFICATORS "-JCPP=${SDL_CPP_OUTPUT_DIRECTORY}/${ARG_SOURCE_NAME}.cpp")
		list(APPEND CUSTOM_MODIFICATORS "-JH=${SDL_CPP_OUTPUT_DIRECTORY}/${ARG_SOURCE_NAME}.h")
	else()
		list(APPEND CUSTOM_MODIFICATORS "--auto-join")
	endif()

	list(LENGTH GLOBAL_SDL_SEARCH_PATHS SDL_PATHS_COUNT)
	if (SDL_PATHS_COUNT GREATER 0)
		foreach(SDL_SEARCH_PATH ${GLOBAL_SDL_SEARCH_PATHS})
			list(APPEND CUSTOM_MODIFICATORS "-H${SDL_SEARCH_PATH}")
		endforeach()
	else()
		message(FATAL_ERROR "SDL PATHS is empty!!!!")
	endif()

	list(LENGTH GLOBAL_SDL_SCHEMA_SEARCH_PATHS SDL_SCHEMA_PATHS_COUNT)
	if (SDL_SCHEMA_PATHS_COUNT GREATER 0 )
		foreach(SDL_SEARCH_PATH ${GLOBAL_SDL_SCHEMA_SEARCH_PATHS})
			list(APPEND CUSTOM_MODIFICATORS "-I${SDL_SEARCH_PATH}")
		endforeach()
	endif()

	ImtCoreGetSdlDeps(
		INPUT
			"${ARG_SCHEMA_PATH}"
		OUT_DIR
			"${SDL_OUTPUT_DIRECTORY}"
		MODIFICATORS
			"${CUSTOM_MODIFICATORS}"
		RESULT_VARIABLE
			FOUND_DEPS)

	set(${ARG_FOUND_DEPS} "${FOUND_DEPS}")

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
			${SDL_GENERATOR_EXE_PATH} ${ARG_SCHEMA_PATH} "${FOUND_SCHEMA_DEPS}" "${ARG_EXTRA_DEPS}"
		COMMENT
			"[CPP:SDL::${PROJECT_NAME}] Creating classes for schema ${ARG_SCHEMA_PATH}"
		VERBATIM)

	target_sources(${PROJECT_NAME} PRIVATE ${FOUND_DEPS})

	message(VERBOSE "EXEC: ${SDL_GENERATOR_EXE_PATH} -GS ${ARG_SCHEMA_PATH} -O ${SDL_OUTPUT_DIRECTORY} ${CUSTOM_MODIFICATORS}")

endmacro()


macro (ImtCoreCustomConfigureSdlQml)
	set(oneValueArgs
		SCHEMA_PATH
			VERSION
			QML_NAME
			EXTRA_DEPS
			FOUND_DEPS
			CUSTOM_OUTPUT_ROOT_DIR_PATH)
	cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})


	set (SDL_OUTPUT_ROOT_DIRECTORY "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL")
	if (ARG_CUSTOM_OUTPUT_ROOT_DIR_PATH)
		set (SDL_OUTPUT_ROOT_DIRECTORY "${ARG_CUSTOM_OUTPUT_ROOT_DIR_PATH}")
	endif()

	set(SDL_OUTPUT_DIRECTORY "${SDL_OUTPUT_ROOT_DIRECTORY}/${ARG_VERSION}/QML/${ARG_QML_NAME}")

	set(MODIFICATORS)
	list(APPEND MODIFICATORS "--QML")
	list(APPEND MODIFICATORS "--use-all-modificators")
	list(APPEND MODIFICATORS "--auto-link=2") ##< Compile the schema provided exclusively.

	# use cache file if provided
	if (GLOBAL_SDL_CACHE_FILE)
		list(APPEND MODIFICATORS "-C${GLOBAL_SDL_CACHE_FILE}")
	endif()

	if (ARG_QML_NAME)
		list(APPEND MODIFICATORS "-N=${ARG_QML_NAME}")
	else()
		list(APPEND MODIFICATORS "--auto-join")
	endif()

	list(LENGTH GLOBAL_SDL_SCHEMA_SEARCH_PATHS SDL_SCHEMA_PATHS_COUNT)
	if (SDL_SCHEMA_PATHS_COUNT GREATER 0 )
		foreach(SDL_SEARCH_PATH ${GLOBAL_SDL_SCHEMA_SEARCH_PATHS})
			list(APPEND MODIFICATORS "-I${SDL_SEARCH_PATH}")
		endforeach()
	endif()

	ImtCoreGetSdlDeps(
		INPUT
			"${ARG_SCHEMA_PATH}"
		OUT_DIR
			"${SDL_OUTPUT_DIRECTORY}"
		MODIFICATORS
			"${MODIFICATORS}"
		RESULT_VARIABLE
			FOUND_DEPS)

	set(${ARG_FOUND_DEPS} "${FOUND_DEPS}")

	ImtCoreGetSdlDeps(GET_SCHEMA_DEPS
		INPUT
			"${ARG_SCHEMA_PATH}"
		OUT_DIR
			"${SDL_OUTPUT_DIRECTORY}"
		MODIFICATORS
			"${MODIFICATORS}"
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
			${SDL_GENERATOR_EXE_PATH} ${ARG_SCHEMA_PATH} "${FOUND_SCHEMA_DEPS}" "${ARG_EXTRA_DEPS}"
		COMMENT
			"[QML:SDL::${PROJECT_NAME}] Creating resources for schema ${ARG_SCHEMA_PATH}"
		VERBATIM)

	message(VERBOSE "EXEC: ${SDL_GENERATOR_EXE_PATH} -GS ${ARG_SCHEMA_PATH} -O ${SDL_OUTPUT_DIRECTORY} ${MODIFICATORS}")

	ImtFutureResourceWrap (
		${PROJECT_NAME}
		"${FOUND_DEPS}"
	)
endmacro()


#! CPP+QML+GQL \NOTE this function enables ALL modificators
#! \param SCHEMA_PATH		- The absolute path to the schema file to be compiled.
#! \param QML_NAME			- \DEPRECATED \WARNING NEVER USE IT! set \param 'QML' instead (boolean)
macro (ImtCoreCustomConfigureSdlCppQml)
	set(oneValueArgs
		SCHEMA_PATH
		VERSION
		QML_NAME
		CUSTOM_OUTPUT_ROOT_DIR_PATH)

	set(booleanArgs
		CXX
		QML)
	cmake_parse_arguments(ARG "${booleanArgs}" "${oneValueArgs}" "" ${ARGN})

	if (ARG_CXX)
		ImtCoreCustomConfigureSdlCpp(
			SCHEMA_PATH "${ARG_SCHEMA_PATH}"
			VERSION "${ARG_VERSION}"
			CUSTOM_OUTPUT_ROOT_DIR_PATH "${ARG_CUSTOM_OUTPUT_ROOT_DIR_PATH}")
	endif()

	if (ARG_QML_NAME)
		# \TODO use FATAL_ERROR for it
		message(WARNING "DEPRECATED argument provided! replace 'QML_NAME MyQmlSdl' to 'QML' ")
		message(VERBOSE "Additing SDL for QML compile source '${ARG_SCHEMA_PATH}' for ${PROJECT_NAME}")
		ImtCoreCustomConfigureSdlQml(
			SCHEMA_PATH "${ARG_SCHEMA_PATH}"
			VERSION "${ARG_VERSION}"
			QML_NAME "${ARG_QML_NAME}"
			CUSTOM_OUTPUT_ROOT_DIR_PATH "${ARG_CUSTOM_OUTPUT_ROOT_DIR_PATH}")
	endif()

	if (ARG_QML)
		message(VERBOSE "Additing SDL for QML compile source '${ARG_SCHEMA_PATH}' for ${PROJECT_NAME}")
		ImtCoreCustomConfigureSdlQml(
			SCHEMA_PATH "${ARG_SCHEMA_PATH}"
			VERSION "${ARG_VERSION}"
			CUSTOM_OUTPUT_ROOT_DIR_PATH "${ARG_CUSTOM_OUTPUT_ROOT_DIR_PATH}")
	endif()

endmacro()
