project(ImtRestPck LANGUAGES CXX)

message("QT_DEFAULT_MAJOR_VERSION " ${QT_DEFAULT_MAJOR_VERSION})

find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS WebSockets REQUIRED)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

target_link_libraries(${PROJECT_NAME} Qt${QT_DEFAULT_MAJOR_VERSION}::WebSockets)

target_link_libraries(${PROJECT_NAME} imtbase imtrest)


if (WIN32)
	set(files  \"${IMTCOREDIR}/Bin/${CMAKE_BUILD_TYPE}${TARGETNAME}/${PROJECT_NAME}.arp\")
	
	windeploy(${PROJECT_NAME} "" "${files}")
endif ()
