if(WIN32)
	target_link_libraries(${PROJECT_NAME} PUBLIC advapi32 user32)
endif()

if(NOT ACF_MODERN_CMAKE)
	include(${ACFDIR}/Config/CMake/AcfStd.cmake)
endif()

