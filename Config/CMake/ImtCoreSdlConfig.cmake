function (ImtCoreConfigureSdlBase
		SDL_FILE_NAME
		OUTPUT_SUBDIR
		NAMESPACE
		MODIFICATORS)

	include (${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ImtCoreSdl.cmake)

	ImtCoreGenerateSdl(
		"${CMAKE_CURRENT_LIST_DIR}/../SDL/${SDL_FILE_NAME}.sdl"
		"${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL/${OUTPUT_SUBDIR}"
		"${NAMESPACE}"
		"${MODIFICATORS}")
endfunction()


#CPP

function (ImtCoreConfigureSdlCpp
		SDL_FILE_NAME)

	ImtCoreConfigureSdlBase(
		"${SDL_FILE_NAME}"
		"CPP"
		"${PROJECT_NAME}::sdl"
		"--CPP;--use-all-modificators")
endfunction()

# CPP+GQL

function (ImtCoreConfigureSdlCppGql
		SDL_FILE_NAME)

	ImtCoreConfigureSdlBase(
		"${SDL_FILE_NAME}"
		"CPP"
		"${PROJECT_NAME}::sdl"
		"--CPP;--GQL;--use-all-modificators")
endfunction()


# QML
function (ImtCoreConfigureSdlQmlBase
		SDL_FILE_NAME
		OUTPUT_SUBDIR
		QML_MODULE_NAME)

	ImtCoreConfigureSdlBase(
		"${SDL_FILE_NAME}"
		"${OUTPUT_SUBDIR}/${QML_MODULE_NAME}"
		"${QML_MODULE_NAME}"
		"--QML;--use-all-modificators")

	ImtCoreFutureResourceWrap (
		${PROJECT_NAME}
		"${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL/${OUTPUT_SUBDIR}/${QML_MODULE_NAME}/${QML_MODULE_NAME}.qrc")
endfunction()


function (ImtCoreConfigureSdlQml
		SDL_FILE_NAME)

	ImtCoreConfigureSdlQmlBase(
		"${SDL_FILE_NAME}"
		"QML"
		"${PROJECT_NAME}Sdl")

endfunction()


function (ImtCoreConfigureSdl 
	SDL_FILE_NAME)

	ImtCoreConfigureSdlCppGql(${SDL_FILE_NAME})
	ImtCoreConfigureSdlQml(${SDL_FILE_NAME})

endfunction()


