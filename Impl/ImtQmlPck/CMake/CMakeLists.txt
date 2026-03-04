project(ImtQmlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::QuickWidgets)
target_link_libraries(${PROJECT_NAME} imtqml imtbasesdl)
target_link_libraries(${PROJECT_NAME} imtqml imtgui imtgql imtauth imtdb imtserverapp imtcom imtbase imtservergql)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} User32 Advapi32)

	set(files \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.arp\")
	windeploy(${PROJECT_NAME} "" "${files}")
endif()

