include(${IACFDIR}/Config/CMake/zlib.cmake)

include_directories(${QUAZIP})

add_compile_definitions(QUAZIP_STATIC)


if(WIN32)
	target_link_libraries(${PROJECT_NAME} ${QUAZIP}/lib/${CMAKE_BUILD_TYPE}${TARGETNAME}/quazip.lib)
endif()

if(UNIX)
	target_link_libraries(${PROJECT_NAME} ${QUAZIP}/lib/${TARGETNAME}/libquazip.a)
endif()


