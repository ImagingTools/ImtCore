include_directories(${OPENSSLDIR})

if(WIN32)
    if(${CMAKE_CL_64} STREQUAL 1)
        target_link_libraries(${PROJECT_NAME} ${OPENSSLDIR}/lib/x64/libcrypto.lib)
    else()
        target_link_libraries(${PROJECT_NAME} ${OPENSSLDIR}/lib/x86/libcrypto.lib)
    endif()
elseif(ANDROID)
    target_link_libraries(${PROJECT_NAME} ${OPENSSLDIR}/lib/android/libcrypto.so)
    set(ANDROID_EXTRA_LIBS
        ${OPENSSLDIR}/lib/android/libcrypto.so
        CACHE INTERNAL "")
    message("ANDROID_EXTRA_LIBS ${ANDROID_EXTRA_LIBS}")
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} ${OPENSSLDIR}/lib/${TARGETNAME}/libcrypto.a)

elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} -lcrypto)
endif()

