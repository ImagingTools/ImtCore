if (USE_FIND_PACKAGE)
	find_package(QuaZip REQUIRED)
	find_package(ZLIB)
	target_link_libraries(${PROJECT_NAME} ${ACF_LIBRARY_LINK_SCOPE} QuaZip::QuaZip ZLIB::ZLIB)
else()
	include_directories(${QUAZIP})

	add_compile_definitions(QUAZIP_STATIC)

	if(WIN32)
		target_link_libraries(${PROJECT_NAME} ${ACF_LIBRARY_LINK_SCOPE} ${QUAZIP}/lib/${CMAKE_BUILD_TYPE}_${TARGETNAME}/quazip.lib)
	endif()

	if(UNIX)
		target_link_libraries(${PROJECT_NAME} ${ACF_LIBRARY_LINK_SCOPE} ${QUAZIP}/lib/${TARGETNAME}/libquazip.a)
	endif()

	include(${IACFDIR}/Config/CMake/zlib.cmake)
endif()
