include(${ACFDIR}/Config/CMake/AcfStd.cmake)

if(TARGET AcfSlnLoc)
	add_compile_definitions(IMT_HAS_ACFSLN_LOC)
endif()

if(WIN32)
	target_link_libraries(${PROJECT_NAME} advapi32 user32)
endif()
