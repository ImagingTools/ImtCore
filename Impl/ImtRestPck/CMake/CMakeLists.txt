project(ImtRestPck LANGUAGES CXX)

message("QT_VERSION_MAJOR " ${QT_VERSION_MAJOR})

find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS WebSockets REQUIRED)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)


target_link_libraries(${PROJECT_NAME} imtrepo imtrest imtbase idoc)
target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::WebSockets)



if (WIN32)
	set(files  \"${IMTCOREDIR}/Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.arp\")
	
	windeploy(${PROJECT_NAME} "" "${files}")
endif ()
