include_directories(${IMTCOREDIR}/3rdParty/EJ/include)
include_directories(${IMTCOREDIR}/3rdParty/EJ/EjExport/mini_zipper/)
include(${IACFDIR}/Config/CMake/zlib.cmake)


if(WIN32)
	target_link_libraries(${PROJECT_NAME} "${IMTCOREDIR}/3rdParty/EJ/libs/EjExport.lib")
endif()

