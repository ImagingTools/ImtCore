include(${IACFDIR}/Config/CMake/zlib.cmake)

include_directories(${QUAZIP}/include)

target_link_libraries(${PROJECT_NAME} ${QUAZIP}/lib/${CMAKE_BUILD_TYPE}${TARGETNAME}/quazip.lib)

add_compile_definitions(QUAZIP_STATIC)
