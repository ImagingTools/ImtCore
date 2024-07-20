include_directories(${IMTCOREDIR}/3rdParty/libmdbx/include)

if(WIN32)
        target_link_libraries(${PROJECT_NAME} "${IMTCOREDIR}/3rdParty/libmdbx/lib/${CMAKE_BUILD_TYPE}_${TARGETNAME}/mdbx.lib")
elseif(UNIX)
        target_link_libraries(${PROJECT_NAME} "${IMTCOREDIR}/3rdParty/libmdbx/lib/${COMPILER_CODE}/libmdbx.so")
endif()
