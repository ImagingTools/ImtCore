	include_directories(${OPENSSLDIR})

if(WIN32)
	if(${CMAKE_CL_64} STREQUAL 1)
	target_link_libraries(${PROJECT_NAME} ${OPENSSLDIR}/lib/x64/libcrypto.lib)
	else()
	target_link_libraries(${PROJECT_NAME} ${OPENSSLDIR}/lib/x86/libcrypto.lib)	
    endif()
endif()

if(UNIX)
	target_link_libraries(${PROJECT_NAME} ${OPENSSLDIR}/lib/${TARGETNAME}/libcrypto.a)
endif()
