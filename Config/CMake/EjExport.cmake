include_directories(${IMTCOREDIR}/3rdParty/EJ/include)
include_directories(${IMTCOREDIR}/3rdParty/EJ/EjExport/mini_zipper/)



if(WIN32)
	target_link_libraries(${PROJECT_NAME} "${IMTCOREDIR}/3rdParty/EJ/libs/${CMAKE_BUILD_TYPE}_${TARGETNAME}/EjExport.lib")
elseif(UNIX)
	target_link_libraries(${PROJECT_NAME} "${IMTCOREDIR}/3rdParty/EJ/libs/${CMAKE_BUILD_TYPE}_${TARGETNAME}/libEjExport.a")
endif()

