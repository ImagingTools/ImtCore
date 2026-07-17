if(WIN32)
	target_link_libraries(${PROJECT_NAME} ${ACF_LIBRARY_LINK_SCOPE} advapi32 user32)
endif()

if(NOT ACF_MODERN_CMAKE)
	include(${ACFDIR}/Config/CMake/AcfStd.cmake)
endif()

