project(controlsgalleryqml LANGUAGES CXX)

include(${IMTCOREDIR}/Config/CMake/WebCompiler.cmake)
include(${ACFDIR}/Config/CMake/StaticConfig.cmake)

if(QT_VERSION_MAJOR EQUAL 6)
	target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core5Compat)
endif()

set(buildwebdir ${BUILDDIR}/ImtControlsGallery/Bin/web)

# copy prolife qml from to
list(APPEND webdirs ${BUILDDIR}/ImtControlsGallery/Qml/controlsgalleryqml)
list(APPEND webdirs ${buildwebdir}/src)

list(APPEND webdirs ${BUILDDIR}/ImtControlsGallery/Qml/controlsgalleryqml/Resources/html)
list(APPEND webdirs ${buildwebdir}/Resources)

# copy base qml sources from to
getImtCoreQmlWebDirs(webdirs "${buildwebdir}")

list(APPEND webdirs ${AUX_INCLUDE_DIR}/controlsgallerysdl/SDL/1.0/QML/controlsgalleryContactInfosSdl)
list(APPEND webdirs ${buildwebdir}/src/controlsgalleryContactInfosSdl)

list(APPEND webdirs ${AUX_INCLUDE_DIR}/controlsgallerysdl/SDL/1.0/QML/controlsgalleryContactInfoCollectionDocumentManagerSdl)
list(APPEND webdirs ${buildwebdir}/src/controlsgalleryContactInfoCollectionDocumentManagerSdl)

list(APPEND webdirs ${AUX_INCLUDE_DIR}/controlsgallerysdl/SDL/1.0/QML/controlsgalleryParamsSetTestSdl)
list(APPEND webdirs ${buildwebdir}/src/controlsgalleryParamsSetTestSdl)

if(DEFINED WEB_COMPILE AND WEB_COMPILE)
	add_compile_definitions(WEB_COMPILE)
	set(resname "controlsgallery")
	jqml_compile_web2("${webdirs}" "${buildwebdir}" "${resname}")
endif()

