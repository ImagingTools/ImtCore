if (USE_FIND_PACKAGE)
	find_package(QuaZip REQUIRED)
	find_package(ZLIB)
	target_link_libraries(${PROJECT_NAME} PUBLIC QuaZip::QuaZip ZLIB::ZLIB)
else()
	include_directories(${QUAZIP})

	add_compile_definitions(QUAZIP_STATIC)

	if(WIN32)
		target_link_libraries(${PROJECT_NAME} PUBLIC ${QUAZIP}/lib/${CMAKE_BUILD_TYPE}_${TARGETNAME}/quazip.lib)
	endif()

	if(UNIX)
		target_link_libraries(${PROJECT_NAME} PUBLIC ${QUAZIP}/lib/${TARGETNAME}/libquazip.a)
	endif()

	include(${IACFDIR}/Config/CMake/zlib.cmake)
endif()
