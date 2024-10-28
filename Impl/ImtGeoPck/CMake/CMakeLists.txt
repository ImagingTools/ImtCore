project(ImtGeoPck LANGUAGES CXX)

message("QT_VERSION_MAJOR " ${QT_VERSION_MAJOR})


include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

target_link_libraries(${PROJECT_NAME} imtgeo imtbase imtdb imtguigql imtgql imtservergql)

include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)

if(WIN32)
	set(files  \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.arp\")
	windeploy(${PROJECT_NAME} "" "${files}")
endif()
