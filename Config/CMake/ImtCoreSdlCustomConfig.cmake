include (${CMAKE_CURRENT_LIST_DIR}/ImtSdlConfig.cmake)
set(IMT_CONFIG_CMAKE_PATH "${CMAKE_CURRENT_LIST_DIR}/..")


macro(ImtCoreGetSdlDeps)
	set(booleanArgs)
	set(oneValueArgs INPUT OUT_DIR RESULT_VARIABLE)
	set(multiValueArgs MODIFICATORS)
	cmake_parse_arguments(ARG "${booleanArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	GetSdlGeneratorPath(SDL_GENERATOR_EXE_PATH)

	set (CUSTOM_PYTHON OFF)
	if (NOT PYTHONEXE)
		set (CUSTOM_PYTHON ON)
		set(PYTHONEXE ${IMTCOREDIR}/3rdParty/Python/3.8/python.exe)

		if (NOT WIN32 AND NOT ANDROID)
			set(PYTHONEXE python3)
		endif()
	endif()

	set(SDL_DEPS_GENERATION_COMMAND ${PYTHONEXE} ${IMT_CONFIG_CMAKE_PATH}/Python/SdlFileNameExtractor.py -S ${ARG_INPUT} -O ${ARG_OUT_DIR} ${ARG_MODIFICATORS})
	set(SDL_ERRORS_FILE_PATH "${ARG_OUT_DIR}/__SDL__DependsList_errors.txt")


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
	if (CUSTOM_PYTHON)
		unset(PYTHONEXE)
	endif()
	unset(CUSTOM_PYTHON)

endmacro()


#  TODO remove it later
function(ImtCoreAddSdlSearchPath ARG_SDL_PATH)
	message(FATAL_ERROR "Outdated function call. Use 'ImtCoreAddSdlHeaderIncludePath'")
endfunction()

function(ImtCoreAddModuleWithHeaders ARG_SDL_PATH)
	ImtCoreAddSdlModulesPath(${ARG_SDL_PATH})
	ImtCoreAddSdlHeaderIncludePath(${ARG_SDL_PATH})
endfunction()

function(ImtCoreAddSdlModulesPath ARG_SDL_PATH)
	if (NOT GLOBAL_IMT_SDL_MODULES_SEARCH_PATHS)
		set(GLOBAL_IMT_SDL_MODULES_SEARCH_PATHS ${ARG_SDL_PATH}
			CACHE
			STRING "List of directories to search for SDL modules, used by SDL code generator"
			FORCE
		)
	else()
		list(APPEND GLOBAL_IMT_SDL_MODULES_SEARCH_PATHS ${ARG_SDL_PATH})
		list(REMOVE_DUPLICATES GLOBAL_IMT_SDL_MODULES_SEARCH_PATHS)
		set(GLOBAL_IMT_SDL_MODULES_SEARCH_PATHS ${GLOBAL_IMT_SDL_MODULES_SEARCH_PATHS}
			CACHE
			STRING "List of directories to search for SDL modules, used by SDL code generator"
			FORCE
		)
	endif()
endfunction()


function(ImtCoreAddSdlHeaderIncludePath ARG_SDL_PATH)
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

	get_filename_component(SCHEMA_NAME "${ARG_SCHEMA_PATH}" NAME_WE)

	set (SDL_OUTPUT_ROOT_DIRECTORY "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL")
	if (ARG_CUSTOM_OUTPUT_ROOT_DIR_PATH)
		set (SDL_OUTPUT_ROOT_DIRECTORY "${ARG_CUSTOM_OUTPUT_ROOT_DIR_PATH}")
	endif()

	ImtCoreAddSdlModulesPath(${SDL_OUTPUT_ROOT_DIRECTORY})
	set(SDL_OUTPUT_DIRECTORY "${SDL_OUTPUT_ROOT_DIRECTORY}")
	set(SDL_CPP_OUTPUT_DIRECTORY "${SDL_OUTPUT_ROOT_DIRECTORY}/${ARG_VERSION}/CPP")
	set(DEP_FILE_PATH "${SDL_CPP_OUTPUT_DIRECTORY}/${SCHEMA_NAME}.depfile")

	set(CUSTOM_MODIFICATORS)
	list(APPEND CUSTOM_MODIFICATORS "--GG=2") ##< Compile, using a new version of code generator.
	list(APPEND CUSTOM_MODIFICATORS "--CPP")
	list(APPEND CUSTOM_MODIFICATORS "--GQL")
	list(APPEND CUSTOM_MODIFICATORS "--use-all-modificators")
	list(APPEND CUSTOM_MODIFICATORS "-Psdl")
	# BUG Base extending error TODO FIXIT
	# list(APPEND CUSTOM_MODIFICATORS "-Bistd::IPolymorphic=istd/IPolymorphic.h")
	list(APPEND CUSTOM_MODIFICATORS "--auto-link=2") ##< Compile the schema provided exclusively.
	list(APPEND CUSTOM_MODIFICATORS "--generator=DEPFILE:${DEP_FILE_PATH}") ##< use depfile
	list(APPEND CUSTOM_MODIFICATORS "--generate-module") ##< add .jsmi module

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
		message(FATAL_ERROR "SDL PATHS is empty! Did you forget add it? Use 'ImtCoreAddSdlHeaderIncludePath' function.")
	endif()

	list(LENGTH GLOBAL_SDL_SCHEMA_SEARCH_PATHS SDL_SCHEMA_PATHS_COUNT)
	if (SDL_SCHEMA_PATHS_COUNT GREATER 0 )
		foreach(SDL_SEARCH_PATH ${GLOBAL_SDL_SCHEMA_SEARCH_PATHS})
			list(APPEND CUSTOM_MODIFICATORS "-I${SDL_SEARCH_PATH}")
		endforeach()
	endif()

	list(LENGTH GLOBAL_IMT_SDL_MODULES_SEARCH_PATHS GLOBAL_IMT_SDL_MODULES_SEARCH_PATHS_COUNT)
	if (GLOBAL_IMT_SDL_MODULES_SEARCH_PATHS_COUNT GREATER 0)
		foreach(SDL_SEARCH_PATH ${GLOBAL_IMT_SDL_MODULES_SEARCH_PATHS})
			list(APPEND CUSTOM_MODIFICATORS "-E${SDL_SEARCH_PATH}")
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

	set(SDL_HEADERS)

	# foreach(SDL_HEADER ${FOUND_DEPS})
	# 	get_filename_component
	# 	list(APPEND SDL_HEADERS "-E${SDL_SEARCH_PATH}")
	# endforeach()

	message(VERBOSE "CPP: FOUND for ${PROJECT_NAME} DEPS: ${FOUND_DEPS}")

	GetSdlGeneratorPath(SDL_GENERATOR_EXE_PATH)

	add_custom_command(
		OUTPUT
			${FOUND_DEPS}
		COMMAND
			${SDL_GENERATOR_EXE_PATH}
		ARGS
			-GS ${ARG_SCHEMA_PATH} -O "${SDL_OUTPUT_DIRECTORY}" ${CUSTOM_MODIFICATORS}
		DEPENDS
			${SDL_GENERATOR_EXE_PATH} "${ARG_EXTRA_DEPS}"
		DEPFILE
			${DEP_FILE_PATH}
		COMMENT
			"[CPP:SDL::${PROJECT_NAME}] Creating classes for schema ${ARG_SCHEMA_PATH}"
		VERBATIM)

	target_sources(${PROJECT_NAME} PRIVATE ${FOUND_DEPS})

	set(HEADER_FILES)

	foreach(SDL_HEADER ${FOUND_DEPS})
		string(FIND "${SDL_HEADER}" ".h" matchres)

		if (${matchres} GREATER  -1)

			# message("acf_create_moc ${SDL_HEADER}")
			LIST(APPEND HEADER_FILES ${SDL_HEADER})
			# acf_create_moc(SDL_HEADER)
			# target_sources(${PROJECT_NAME} PRIVATE ${FOUND_DEPS})
		endif ()
	endforeach()

	message("acf_create_moc ${HEADER_FILES}")

	# acf_create_moc(MOC_SOURCES "${HEADER_FILES}")
	# qt6_wrap_cpp("${outfiles}" ${ARGN} )
	qt6_wrap_cpp(MOC_SOURCES "${HEADER_FILES}")
	target_sources(${PROJECT_NAME} PRIVATE ${MOC_SOURCES})

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

	if (ARG_QML_NAME)
		list(APPEND MODIFICATORS "-N=${ARG_QML_NAME}")
	else()
		set(SDL_OUTPUT_DIRECTORY "${SDL_OUTPUT_ROOT_DIRECTORY}")
		list(APPEND MODIFICATORS "--auto-join")
	endif()

	get_filename_component(SCHEMA_NAME "${ARG_SCHEMA_PATH}" NAME_WE)
	set(DEP_FILE_PATH "${SDL_OUTPUT_DIRECTORY}/${ARG_VERSION}/QML/${SCHEMA_NAME}.depfile")

	list(APPEND MODIFICATORS "--QML")
	list(APPEND MODIFICATORS "--use-all-modificators")
	list(APPEND MODIFICATORS "--auto-link=2") ##< Compile the schema provided exclusively.
	list(APPEND MODIFICATORS "--generator=DEPFILE:${DEP_FILE_PATH}") ##< use depfile

	# use cache file if provided
	if (GLOBAL_SDL_CACHE_FILE)
		list(APPEND MODIFICATORS "-C${GLOBAL_SDL_CACHE_FILE}")
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

	message(VERBOSE "QML DEP FOUND for ${PROJECT_NAME} DEPS: ${FOUND_DEPS} ")

	GetSdlGeneratorPath(SDL_GENERATOR_EXE_PATH)
	add_custom_command(
		OUTPUT
			${FOUND_DEPS}
		COMMAND
			${SDL_GENERATOR_EXE_PATH}
		ARGS
			-GS ${ARG_SCHEMA_PATH} -O "${SDL_OUTPUT_DIRECTORY}" ${MODIFICATORS}
		DEPENDS
			${SDL_GENERATOR_EXE_PATH} "${ARG_EXTRA_DEPS}"
		DEPFILE
			${DEP_FILE_PATH}
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
#! \param QML_NAME			- \DEPRECATED \WARNING NEVER USE IT! set \param 'QML' instead (boolean) \TODO After adapting all the projects, remove it.
#! \TODO think about \param SCHEMA_PATH as multi value
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
		# \TODO REMOVE IT later
		message(FATAL_ERROR "DEPRECATED argument provided! replace 'QML_NAME MyQmlSdl' to 'QML' ")

		# unreachable code!
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
