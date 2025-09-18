project(ImtDatabasePck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtdb imtauth iser imtcol)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)

if(WIN32)

	list(APPEND listOptions "")
	set(files \"${IMTCOREDIR_BUILD}/Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.arp\")

	windeploy(${PROJECT_NAME} "${listOptions}" "${files}")
	unset(listOptions)
endif()
