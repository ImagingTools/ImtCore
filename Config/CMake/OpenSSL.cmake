include_directories(${OPENSSLDIR})

if(WIN32)
    if(${CMAKE_CL_64} STREQUAL 1)
        target_link_libraries(${PROJECT_NAME} ${OPENSSLDIR}/lib/x64/libcrypto.lib)
    else()
        target_link_libraries(${PROJECT_NAME} ${OPENSSLDIR}/lib/x86/libcrypto.lib)
    endif()
elseif(ANDROID)
    target_link_libraries(${PROJECT_NAME} ${OPENSSLDIR}/lib/android/libcrypto_1_1.so)
    target_link_libraries(${PROJECT_NAME} ${OPENSSLDIR}/lib/android/libssl_1_1.so)
    set(ANDROID_EXTRA_LIBS
        ${OPENSSLDIR}/lib/android/libcrypto_1_1.so
        ${OPENSSLDIR}/lib/android/libssl_1_1.so
        CACHE INTERNAL "")
    message("ANDROID_EXTRA_LIBS ${ANDROID_EXTRA_LIBS}")
elseif(APPLE)
	if("${CMAKE_OSX_ARCHITECTURES}" STREQUAL "arm64")
		target_link_libraries(${PROJECT_NAME} ${OPENSSLDIR}/lib/ClangOSX_arm64/libcrypto.a)
	else()
		target_link_libraries(${PROJECT_NAME} ${OPENSSLDIR}/lib/ClangOSX_x64/libcrypto.a)
	endif()

elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} -lcrypto)
endif()

