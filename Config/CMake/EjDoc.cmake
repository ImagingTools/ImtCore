include_directories(${IMTCOREDIR}/3rdParty/EJ/include)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} "${IMTCOREDIR}/3rdParty/EJ/libs/EjDoc.lib")
endif()
