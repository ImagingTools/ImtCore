project(graphqlclient LANGUAGES CXX)

set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/ImtCore.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")


if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "on")
endif()

# add_library(${PROJECT_NAME} SHARED ../Main.cpp)

include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/GeneralConfig.cmake)
include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
# include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
# include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)
include(${IMTCOREDIR}/Config/CMake/Quazip.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES_FILE_AUX})

# set(outlibdir "${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}")

# set_target_properties( ${PROJECT_NAME}
# 	PROPERTIES
# 	ARCHIVE_OUTPUT_DIRECTORY ${outlibdir}
# 	LIBRARY_OUTPUT_DIRECTORY ${outlibdir}
# 	RUNTIME_OUTPUT_DIRECTORY ${outlibdir}
# 	POSITION_INDEPENDENT_CODE ON
# )

target_link_libraries(${PROJECT_NAME} iservice)
target_link_libraries(${PROJECT_NAME} imtbase imtgui imtauth imtauthgui imtlicdb imtlic imtlicgui imtrest imtrepo imtstyle imtqml imtdb imtfile imtcom imtlicsdl imtappsdl imtauthsdl imtbasesdl imtservergql imtcol)
target_link_libraries(${PROJECT_NAME} imtlicgql imtguigql imtclientgql imtgql imtauthgql imtauthdb imtapp imtservice imtlog)
target_link_libraries(${PROJECT_NAME} ImtCoreLoc)
target_link_libraries(${PROJECT_NAME} imtstylecontrolsqml)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::QuickWidgets  Qt${QT_VERSION_MAJOR}::WebSockets)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(${PROJECT_NAME} imtgui)

include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)

if(DEFINED WEB_COMPILE AND  WEB_COMPILE)
	add_compile_definitions(WEB_COMPILE)
endif()

if(WIN32)
	# list(APPEND listOptions "--qmldir=${IMTCOREDIR}/Include/imtqml/Resources/qml")
	list(APPEND listOptions "--qmldir=${IMTCOREDIR}/Qml")
	set(files  \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.pck\")

	# windeploy(${PROJECT_NAME} "${listOptions}" "${files}")
	unset(listOptions)
endif()

