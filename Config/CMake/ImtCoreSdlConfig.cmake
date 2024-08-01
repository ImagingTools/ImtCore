function (ImtCoreConfigureSdlCpp
		SDL_FILE_NAME
		OUTPUT_SUBDIR
		NAMESPACE
		MODIFICATORS)

	include (${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ImtCoreSdl.cmake)

	ImtCoreGenerateCppSdl(
		"${CMAKE_CURRENT_LIST_DIR}/../SDL/${SDL_FILE_NAME}.sdl"
		"${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL/${OUTPUT_SUBDIR}"
		"${NAMESPACE}"
		"${MODIFICATORS}")
endfunction()

function (ImtCoreConfigureSdlQml
		SDL_FILE_NAME
		OUTPUT_SUBDIR
		NAMESPACE
		MODIFICATORS)

	include (${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ImtCoreSdl.cmake)

	ImtCoreGenerateQmlSdl(
		"${CMAKE_CURRENT_LIST_DIR}/../SDL/${SDL_FILE_NAME}.sdl"
		"${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL/${OUTPUT_SUBDIR}"
		"${NAMESPACE}"
		"${MODIFICATORS}")
endfunction()


#CPP

function (ImtCoreConfigureSdlCppSingle
		SDL_FILE_NAME)

	ImtCoreConfigureSdl(
		"${SDL_FILE_NAME}"
		"CPP"
		"${PROJECT_NAME}::sdl"
		"--CPP;--use-all-modificators")
endfunction()


# CPP+GQL

function (ImtCoreConfigureSdlCppGqlSingle
		SDL_FILE_NAME)

	ImtCoreConfigureSdl(
		"${SDL_FILE_NAME}"
		"CPP"
		"${PROJECT_NAME}::sdl"
		"--CPP;--GQL;--use-all-modificators;-Bistd::IPolymorphic=istd/IPolymorphic.h")
endfunction()


# QML

function (ImtCoreConfigureSdlQml
		SDL_FILE_NAME
		OUTPUT_SUBDIR
		QML_MODULE_NAME)

	ImtCoreConfigureSdl(
		"${SDL_FILE_NAME}"
		"${OUTPUT_SUBDIR}/${QML_MODULE_NAME}"
		"${QML_MODULE_NAME}"
		"--QML;--use-all-modificators")


	include (${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ImtCoreSdl.cmake)
	ImtCoreFutureResourceWrap (
		${PROJECT_NAME}
		"${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL/${OUTPUT_SUBDIR}/${QML_MODULE_NAME}/${QML_MODULE_NAME}.qrc")

	# update QML import paths for Qt creator
	set(QML_IMPORT_DIRS ${QML_IMPORT_PATH})
	list(APPEND QML_IMPORT_DIRS "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL/${OUTPUT_SUBDIR}")
	list(REMOVE_DUPLICATES QML_IMPORT_DIRS)
	set(QML_IMPORT_PATH ${QML_IMPORT_DIRS}
		CACHE STRING "Qt Creator extra qml import paths for ${PROJECT_NAME}"
		FORCE
	)

endfunction()


# QML single

function (ImtCoreConfigureSdlQmlSingle
		SDL_FILE_NAME)

	ImtCoreConfigureSdlQml(
		"${SDL_FILE_NAME}"
		"QML"
		"${PROJECT_NAME}Sdl")

endfunction()


# Lazy CPP+QML+GQL WARNING this function enables ALL modificators

function (ImtCoreConfigureSdlCppQml
		SDL_FILE_NAME
		CPP_OUT_SUBFOLDER
		CPP_NAMESPACE
		QML_OUT_SUBFOLDER
		QML_MODULE_NAME
	)

ImtCoreConfigureSdl("${SDL_FILE_NAME}" "${CPP_OUT_SUBFOLDER}" "${CPP_NAMESPACE}" "--use-all-modificators;--CPP;--GQL")
ImtCoreConfigureSdlQml("${SDL_FILE_NAME}" "${QML_OUT_SUBFOLDER}" "${QML_MODULE_NAME}")

endfunction()


# Lazy single file CPP+QML+GQL WARNING this function enables ALL modificators

function (ImtCoreConfigureSdlSingle
	SDL_FILE_NAME)

	ImtCoreConfigureSdlCppGqlSingle(${SDL_FILE_NAME})
	ImtCoreConfigureSdlQmlSingle(${SDL_FILE_NAME})

endfunction()


