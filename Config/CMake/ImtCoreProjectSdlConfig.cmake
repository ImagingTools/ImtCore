#! This config file created for internal IMT projects


function (ImtCoreConfigureSdlCpp
		SDL_FILE_NAME
		OUTPUT_SUBDIR
		NAMESPACE
		MODIFICATORS)

	include (${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ImtSdlConfig.cmake)

	set(CUSTOM_MODIFICATORS)
	list(APPEND CUSTOM_MODIFICATORS "${MODIFICATORS}")
	list(APPEND CUSTOM_MODIFICATORS "-Bistd::IPolymorphic=istd/IPolymorphic.h")

	ImtGenerateCppSdl(
		"${CMAKE_CURRENT_LIST_DIR}/../SDL/${SDL_FILE_NAME}.sdl"
		"${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL/${OUTPUT_SUBDIR}"
		"${NAMESPACE}"
		"${CUSTOM_MODIFICATORS}")
endfunction()

function (ImtCoreConfigureSdlQml
		SDL_FILE_NAME
		OUTPUT_SUBDIR
		QML_MODULE_NAME)

	include (${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ImtSdlConfig.cmake)

	ImtGenerateQmlSdl(
		"${CMAKE_CURRENT_LIST_DIR}/../SDL/${SDL_FILE_NAME}.sdl"
		"${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL/${OUTPUT_SUBDIR}/${QML_MODULE_NAME}"
		"${QML_MODULE_NAME}"
		"--QML;--use-all-modificators")

	SET(sdldependency "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL/QML/${QML_MODULE_NAME}/${QML_MODULE_NAME}.qrc")

	ImtFutureResourceWrap (
		${PROJECT_NAME}
		"${sdldependency}"
	)
endfunction()


# CPP+QML+GQL WARNING this function enables ALL modificators

function (ImtCoreConfigureSdlCppQml
		SDL_FILE_NAME
		CPP_OUT_SUBFOLDER
		CPP_NAMESPACE
		QML_OUT_SUBFOLDER
		QML_MODULE_NAME
	)

set(CUSTOM_MODIFICATORS)
	list(APPEND CUSTOM_MODIFICATORS "-I ${IMTCOREDIR}/Sdl")
	list(APPEND CUSTOM_MODIFICATORS "--CPP")
	list(APPEND CUSTOM_MODIFICATORS "--GQL")
	list(APPEND CUSTOM_MODIFICATORS "--use-all-modificators")
	list(APPEND CUSTOM_MODIFICATORS "-Bimtbase::IJsonObjectWrittable=imtbase/IJsonObjectWrittable.h")
	list(APPEND CUSTOM_MODIFICATORS "-Bimtbase::ITreeModelWrittable=imtbase/ITreeModelWrittable.h")
	list(APPEND CUSTOM_MODIFICATORS "-Bimtgql::IGrahpQlWrittable=imtgql/IGrahpQlWrittable.h")

ImtCoreConfigureSdlCpp("${SDL_FILE_NAME}" "${CPP_OUT_SUBFOLDER}" "${CPP_NAMESPACE}" "${CUSTOM_MODIFICATORS}")
ImtCoreConfigureSdlQml("${SDL_FILE_NAME}" "${QML_OUT_SUBFOLDER}" "${QML_MODULE_NAME}")

endfunction()
