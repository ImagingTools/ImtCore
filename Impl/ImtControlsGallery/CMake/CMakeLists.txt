project(ImtControlsGallery LANGUAGES CXX)

message("ACFDIR LISA " ${ACFDIR})
message("CMAKE_PROJECT_INCLUDE_BEFORE " ${CMAKE_PROJECT_INCLUDE_BEFORE})

set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/ImtCore.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

message("ARXC_CONFIG " ${ARXC_CONFIG})
message("ARXC_OUTDIR " ${ARXC_OUTDIR})

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

find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS WebSockets REQUIRED)
find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Quick Qml REQUIRED)


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
target_link_libraries(${PROJECT_NAME} imtbase imtgui imtzip imtrest imtcrypt imtstyle imtqml imtdb imtfile imtguigql)
target_link_libraries(${PROJECT_NAME} imtlicgql imtguigql imtgql imtauthgui imtauthgql imtauthdb imtapp imtstyleqml imtcontrols)
# target_link_libraries(${PROJECT_NAME} lisaqml ImtCoreLoc LisaLoc)
target_link_libraries(${PROJECT_NAME} iqtgui)
target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::Quick  Qt${QT_VERSION_MAJOR}::WebSockets)


message("PROJECT_BINARY_DIR ${IMTCOREDIR}/Bin/web")
set(buildwebdir ${CMAKE_CURRENT_LIST_DIR}/../../../Bin/web)
message("LASA binary dir ${buildwebdir}")

list(APPEND webdirs ${IMTCOREDIR}/3rdParty/JQML/Core/dist)
list(APPEND webdirs ${buildwebdir}/Resources)

# copy lisa qml from to
list(APPEND webdirs ${CMAKE_CURRENT_LIST_DIR}/../Qml)
list(APPEND webdirs ${buildwebdir}/src)

# list(APPEND webdirs ${LISADIR}/Include/lisaqml/Resources/Translations)
# list(APPEND webdirs ${buildwebdir}/src)

list(APPEND webdirs ${CMAKE_CURRENT_LIST_DIR}/../Resources/html)
list(APPEND webdirs ${buildwebdir}/Resources)

# copy base qml sources from to
list(APPEND webdirs ${IMTCOREDIR}/Include/imtgui/Qml/Acf)
list(APPEND webdirs ${buildwebdir}/src/Acf)

list(APPEND webdirs ${IMTCOREDIR}/Include/imtgui/Qml/imtgui)
list(APPEND webdirs ${buildwebdir}/src/imtgui)

list(APPEND webdirs ${IMTCOREDIR}/Include/imtguigql/Qml/imtguigql)
list(APPEND webdirs ${buildwebdir}/src/imtguigql)

list(APPEND webdirs ${IMTCOREDIR}/Include/imtqml/Qml/imtqml)
list(APPEND webdirs ${buildwebdir}/src/imtqml)

list(APPEND webdirs ${IMTCOREDIR}/Include/imtauthgui/Qml/imtauthgui)
list(APPEND webdirs ${buildwebdir}/src/imtauthgui)

list(APPEND webdirs ${IMTCOREDIR}/Include/imtlicgui/Qml/imtlicgui)
list(APPEND webdirs ${buildwebdir}/src/imtlicgui)

list(APPEND webdirs ${IMTCOREDIR}/Include/imtstyleqml/Qml/Acf)
list(APPEND webdirs ${buildwebdir}/src/Acf)

message("LISADIR ${LISADIR}")

# copy dist from to
list(APPEND webdirs ${CMAKE_CURRENT_LIST_DIR}/../Resources/dist)
list(APPEND webdirs ${buildwebdir}/Resources)

# copy result from to
list(APPEND webdirs ${IMTCOREDIR}/Bin/web/build.web)
list(APPEND webdirs ${buildwebdir}/Resources)

if(DEFINED WEB_COMPILE AND  WEB_COMPILE)
	add_compile_definitions(WEB_COMPILE)
	set(resname "controlsgallery")
	jqml_compile_web("${webdirs}", "${buildwebdir}", "${resname}")
endif()



if(WIN32)
	list(APPEND listOptions "--qmldir=${IMTCOREDIR}/Include/imtqml/Resources/qml")
	list(APPEND listOptions "--qmldir=${IMTCOREDIR}/Include/imtgui/Qml/imtgui")
	set(files  \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")

	windeploy(${PROJECT_NAME} "${listOptions}" "${files}")
	unset(listOptions)
endif()

if(DEFINED CONTROLS_QML_DIRS AND CONTROLS_QML_DIRS)
    list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Include/imtqml/Qml")
    list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Include/imtgui/Qml")
    list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Include/imtguigql/Qml")
    list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Include/imtcontrols/Qml")
    list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Include/imtstyleqml/Qml")

    # Prevent adding duplicate values at each run of CMake.
    list(REMOVE_DUPLICATES QML_IMPORT_DIRS)

    # The variable is cached in ${BUILD_DIR}/CMakeCache.txt. We need FORCE to
    # change it there immediately. Also, add a comment to the cache file.
    set(QML_IMPORT_PATH ${QML_IMPORT_DIRS}
        CACHE STRING "Qt Creator extra qml import paths for ${PROJECT_NAME}"
        FORCE
    )
endif()
