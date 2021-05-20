include(${ACFDIR}/Config/CMake/AcfStd.cmake)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} advapi32 user32)
endif()