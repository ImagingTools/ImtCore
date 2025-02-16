project(ImtControlsGallery LANGUAGES CXX)

message(VERBOSE "ACFDIR LISA " ${ACFDIR})
message(VERBOSE "CMAKE_PROJECT_INCLUDE_BEFORE " ${CMAKE_PROJECT_INCLUDE_BEFORE})

set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/ImtCore.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

message(VERBOSE "ARXC_CONFIG " ${ARXC_CONFIG})
message(VERBOSE "ARXC_OUTDIR " ${ARXC_OUTDIR})

if(WIN32)
	set(CMAKE_WIN32_EXECUTABLE "on")
	# set(ACF_CONVERT_FILES "${PROJECT_SOURCE_DIR}/../VC/ImtControlsGallery.rc.xtracf")
	# set(ACF_CONVERT_REGISTRY "${PROJECT_SOURCE_DIR}/../VC/FileSubstitCopyApp.acc")
	# set(ACF_CONVERT_CONFIG  "${PROJECT_SOURCE_DIR}/../../../Config/BaseOnly.awc")
	# set(RC_FILE "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/ImtControlsGallery.rc")
endif()


if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "on")
endif()

include(${IMTCOREDIR}/Config/CMake/WebCompiler.cmake)
include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

if(DEFINED WEB_COMPILE AND  WEB_COMPILE)
	add_compile_definitions(WEB_COMPILE)
endif()

target_link_libraries(${PROJECT_NAME} iauth)
target_link_libraries(${PROJECT_NAME} imtbase imtzip imtrest imtcrypt imtstyle imtqml imtdb imtfile imtguigqlqml imtauthguiqml imtguiqml imtcolguiqml imtauthsdl imtappsdl)
target_link_libraries(${PROJECT_NAME} imtlicgql imtguigql imtgql imtclientgql imtauthgui imtauthgql imtauthdb imtapp imtstylecontrolsqml imtcontrolsqml imtdocguiqml imtgui imtservergql)
target_link_libraries(${PROJECT_NAME} iqtgui)
target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::Quick  Qt${QT_VERSION_MAJOR}::WebSockets)
target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::QuickControls2)

set(buildwebdir ${CMAKE_CURRENT_LIST_DIR}/../../../Bin/web)

list(APPEND webdirs ${CMAKE_CURRENT_LIST_DIR}/../Qml)
list(APPEND webdirs ${buildwebdir}/src)

list(APPEND webdirs ${CMAKE_CURRENT_LIST_DIR}/../Resources/html)
list(APPEND webdirs ${buildwebdir}/Resources)

getImtCoreQmlWebDirs(webdirs "${buildwebdir}")

if(DEFINED WEB_COMPILE AND  WEB_COMPILE)
	add_compile_definitions(WEB_COMPILE)
	set(resname "controlsgallery")
	set(inputjs "${IMTCOREDIR}/Qml/web/imtcore.json")
	jqml_compile_web2("${webdirs}" "${buildwebdir}" "${resname}")
	#jq_compile_web("${webdirs}", "${buildwebdir}", "${resname}", "${inputjs}")
endif()

if(WIN32)
	list(APPEND listOptions "--qmldir=${IMTCOREDIR}/Qml")
	set(files  \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")

	windeploy(${PROJECT_NAME} "${listOptions}" "${files}")
	unset(listOptions)
endif()

set(QML_IMPORT_DIRS ${QML_IMPORT_PATH})

list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Qml")
list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Qml/imtcontrols/Base")

if(DEFINED QML_DIRS_CONTROLS AND QML_DIRS_CONTROLS)
	message(VERBOSE "CONTROLS_QML_DIRS ${CONTROLS_QML_DIRS}")
	list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Include/imtstylecontrolsqml/Qml")
endif()

# Prevent adding duplicate values at each run of CMake.
list(REMOVE_DUPLICATES QML_IMPORT_DIRS)

# The variable is cached in ${BUILD_DIR}/CMakeCache.txt. We need FORCE to
# change it there immediately. Also, add a comment to the cache file.
set(QML_IMPORT_PATH ${QML_IMPORT_DIRS}
	CACHE STRING "Qt Creator extra qml import paths for ${PROJECT_NAME}"
	FORCE
)
