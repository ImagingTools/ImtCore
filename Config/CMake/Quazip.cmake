include(${IACFDIR}/Config/CMake/zlib.cmake)

include_directories(${QUAZIP})

add_compile_definitions(QUAZIP_STATIC)

message("CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}_${TARGETNAME}")
message("QUAZIP ${QUAZIP}")

if(WIN32)
	message("WIN32")
	target_link_libraries(${PROJECT_NAME} ${QUAZIP}/lib/${CMAKE_BUILD_TYPE}_${TARGETNAME}/quazip.lib)
endif()

if(UNIX)
	target_link_libraries(${PROJECT_NAME} ${QUAZIP}/lib/${TARGETNAME}/libquazip.a)
endif()


