project(controlsgalleryqml LANGUAGES CXX)

include(${IMTCOREDIR}/Config/CMake/WebCompiler.cmake)
include(${ACFDIR}/Config/CMake/StaticConfig.cmake)

if(QT_VERSION_MAJOR EQUAL 6)
	target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core5Compat)
endif()

set(buildwebdir ${BUILDDIR}/ImtControlsGallery/Bin/web)

# copy local qml sources from source tree to web build dir
list(APPEND webdirs ${PROJECT_SOURCE_DIR}/..)
list(APPEND webdirs ${buildwebdir}/src)

list(APPEND webdirs ${PROJECT_SOURCE_DIR}/../Resources/html)
list(APPEND webdirs ${buildwebdir}/Resources)

# copy base qml sources from to
getImtCoreQmlWebDirs(webdirs "${buildwebdir}")

list(APPEND webdirs ${AUX_INCLUDE_DIR}/controlsgallerysdl/SDL/1.0/QML/controlsgalleryContactInfosSdl)
list(APPEND webdirs ${buildwebdir}/src/controlsgalleryContactInfosSdl)

list(APPEND webdirs ${AUX_INCLUDE_DIR}/controlsgallerysdl/SDL/1.0/QML/controlsgalleryContactInfoCollectionDocumentServiceSdl)
list(APPEND webdirs ${buildwebdir}/src/controlsgalleryContactInfoCollectionDocumentServiceSdl)

list(APPEND webdirs ${AUX_INCLUDE_DIR}/controlsgallerysdl/SDL/1.0/QML/controlsgalleryParamsSetTestSdl)
list(APPEND webdirs ${buildwebdir}/src/controlsgalleryParamsSetTestSdl)

option(USE_NEW_WEB "Use new web compilation (jq_compile_web)" OFF)

if(DEFINED WEB_COMPILE AND WEB_COMPILE)
	add_compile_definitions(WEB_COMPILE)
	set(resname "controlsgallery")
	if(USE_NEW_WEB)
		set(inputjs "${IMTCOREDIR}/Impl/ImtControlsGallery/Qml/controlsgalleryqml/gallery.json")
		set(appicon "../Icons/GalleryIcon.svg")
		set(dataroot "/ControlsGallery/Views/")
		set(startqml "${IMTCOREDIR}/Impl/ImtControlsGallery/Qml/controlsgalleryqml/ImtControlsGalleryWeb.qml")
		list(APPEND envvar "IMTCOREDIR_BUILD=${IMTCOREDIR_BUILD}")
		list(APPEND envvar "IMTCOREDIR=${IMTCOREDIR}")

		jq_compile_web(
			"${webdirs}"
			"${buildwebdir}"
			"${resname}"
			"${inputjs}"
			"${appicon}"
			"${dataroot}"
			"${startqml}"
			"${envvar}"
		)
else()
		jqml_compile_web2("${webdirs}" "${buildwebdir}" "${resname}")
	endif()
endif()
