project(ImtComPck LANGUAGES CXX)

message("QT_VERSION_MAJOR " ${QT_VERSION_MAJOR})


include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtcom)

if(WIN32)
	set(files  \"${IMTCOREDIR}/Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.arp\")
	
	windeploy(${PROJECT_NAME} "" "${files}")
endif()

