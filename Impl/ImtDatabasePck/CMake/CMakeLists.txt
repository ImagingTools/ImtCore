project(ImtDatabasePck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtdb)

if(WIN32)

	list(APPEND listOptions "")
	set(files  \"${IMTCOREDIR}/Bin/${CMAKE_BUILD_TYPE}${TARGETNAME}/${PROJECT_NAME}.arp\")

	windeploy(${PROJECT_NAME} "${listOptions}" "${files}")
	unset(listOptions)
endif()
