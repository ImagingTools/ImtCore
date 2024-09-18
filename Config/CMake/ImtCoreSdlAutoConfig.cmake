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
function (ImtCoreAutoConfigureSdlCpp
		SDL_FILE_NAME
		MODIFICATORS)

	include (${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ImtSdlConfig.cmake)

	set(CUSTOM_MODIFICATORS)
	list(APPEND CUSTOM_MODIFICATORS "${MODIFICATORS}")
	list(APPEND CUSTOM_MODIFICATORS "-Bistd::IPolymorphic=istd/IPolymorphic.h")

	ImtCoreGetSdlDeps(
		INPUT
			"${SDL_FILE_NAME}"
		OUT_DIR
			"${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL"
		MODIFICATORS
			"${MODIFICATORS}"
		RESULT_VARIABLE
			FOUND_DEPS)

	message (VERBOSE "FOUND for ${PROJECT_NAME} DEPS: ${FOUND_DEPS}")

	GetSdlGeneratorPath(SDL_GENERATOR_EXE_PATH)
	message(VERBOSE "SDL_GENERATOR_EXE_PATH = ${SDL_GENERATOR_EXE_PATH}")
	message(VERBOSE "FOUND_DEPS = ${FOUND_DEPS}")

	add_custom_command(
		OUTPUT
			${FOUND_DEPS}
		COMMAND
			${SDL_GENERATOR_EXE_PATH}
		ARGS
			-GS ${SDL_FILE_PATH} -O "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL" ${CUSTOM_MODIFICATORS}
		DEPENDS
			${SDL_GENERATOR_EXE_PATH} ${SDL_FILE_PATH}
		COMMENT
			"[CPP:SDL::${PROJECT_NAME}] Creating classes"
		VERBATIM)

	target_sources(${PROJECT_NAME} PRIVATE ${FOUND_DEPS})

endfunction()

function (ImtCoreAutoConfigureSdlQml
		SDL_FILE_PATH)

	include (${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ImtSdlConfig.cmake)

	set(MODIFICATORS)
	list(APPEND MODIFICATORS "--QML")
	list(APPEND MODIFICATORS "--use-all-modificators")
	list(APPEND MODIFICATORS "--generator=CMake")
	list(APPEND MODIFICATORS "--auto-join")

	ImtCoreGetSdlDeps(
		INPUT
			"${SDL_FILE_PATH}"
		OUT_DIR
			"${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL"
		MODIFICATORS
			"${MODIFICATORS}"
		RESULT_VARIABLE
			FOUND_DEPS)

	message (VERBOSE "QML DEP FOUND for ${PROJECT_NAME} DEPS: ${FOUND_DEPS}")

	# ImtGenerateQmlSdl(
	# 	"${SDL_FILE_PATH}"
	# 	"${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL"
	# 	"${QML_MODULE_NAME}"
	# 	"--QML;--use-all-modificators;--generator=CMake;--auto-join")

	add_custom_command(
		OUTPUT
			${FOUND_DEPS}
		COMMAND
			${SDL_GENERATOR_EXE_PATH}
		ARGS
			-GS ${SDL_FILE_PATH} -O "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL" ${MODIFICATORS}
		DEPENDS
			${SDL_GENERATOR_EXE_PATH} ${SDL_FILE_PATH}
		COMMENT
			"[QML:SDL::${PROJECT_NAME}] Creating resources"
		VERBATIM)

	ImtFutureResourceWrap (
		${PROJECT_NAME}
		"${FOUND_DEPS}"
	)
endfunction()


# CPP+QML+GQL WARNING this function enables ALL modificators

function (ImtCoreAutoConfigureSdlCppQml
		SDL_SOURCES)

	# target_sources(${PROJECT_NAME} PRIVATE ${SDL_SOURCES})

	set(CUSTOM_MODIFICATORS)
	list(APPEND CUSTOM_MODIFICATORS "--CPP")
	list(APPEND CUSTOM_MODIFICATORS "--GQL")
	list(APPEND CUSTOM_MODIFICATORS "--use-all-modificators")
	list(APPEND CUSTOM_MODIFICATORS "--auto-join")
	list(APPEND CUSTOM_MODIFICATORS "--generator=CMake")
	list(APPEND CUSTOM_MODIFICATORS "-Psdl")

	foreach(SDL_FILE_PATH IN LISTS SDL_SOURCES)
		message(STATUS "Additing SDL source '${SDL_FILE_PATH}' for ${PROJECT_NAME}")

		ImtCoreAutoConfigureSdlCpp(${SDL_FILE_PATH} "${CUSTOM_MODIFICATORS}")
		ImtCoreAutoConfigureSdlQml("${SDL_FILE_PATH}")
	endforeach()

endfunction()
