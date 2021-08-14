project(ImtComPck LANGUAGES CXX)

message("QT_DEFAULT_MAJOR_VERSION " ${QT_DEFAULT_MAJOR_VERSION})


include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

target_link_libraries(${PROJECT_NAME} imtdb imtbase imtcom)

if(WIN32)
	set(files  \"${IMTCOREDIR}/Bin/${CMAKE_BUILD_TYPE}${TARGETNAME}/${PROJECT_NAME}.arp\")
	
	windeploy(${PROJECT_NAME} "" "${files}")
endif()

