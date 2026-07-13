include_directories(${IMTCOREDIR}/3rdParty/EJ/include)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} ${ACF_LIBRARY_LINK_SCOPE} "${IMTCOREDIR}/3rdParty/EJ/libs/${CMAKE_BUILD_TYPE}_${TARGETNAME}/EjDoc.lib")
elseif(UNIX)
	target_link_libraries(${PROJECT_NAME} ${ACF_LIBRARY_LINK_SCOPE} "${IMTCOREDIR}/3rdParty/EJ/libs/${CMAKE_BUILD_TYPE}_${TARGETNAME}/libEjDoc.a")
endif()
