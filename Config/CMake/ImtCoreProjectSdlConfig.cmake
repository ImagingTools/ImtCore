#! This config file created for internal IMT projects


function (ImtCoreConfigureSdlCpp
		SDL_FILE_NAME
		OUTPUT_SUBDIR
		NAMESPACE
		MODIFICATORS)

	include (${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ImtSdlConfig.cmake)

	ImtGenerateCppSdl(
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

	include (${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ImtSdlConfig.cmake)

	ImtGenerateQmlSdl(
		"${CMAKE_CURRENT_LIST_DIR}/../SDL/${SDL_FILE_NAME}.sdl"
		"${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL/${OUTPUT_SUBDIR}"
		"${NAMESPACE}"
		"${MODIFICATORS}")
endfunction()



