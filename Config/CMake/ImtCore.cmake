include(${ACFDIR}/Config/CMake/AcfStd.cmake)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} advapi32 user32)
endif()

function(imtcore_generate_tests pathToClassTest)

	if (NOT BUILD_TESTING)
		message(STATUS "Testing is disabled, skipping test generation for ${pathToClassTest}")

		return()
	endif()

	set(PYTHONEXE "$ENV{PYTHONEXE}")
	
	if(PYTHONEXE STREQUAL "")
		if (WIN32 AND NOT ANDROID)
			set(PYTHONEXE python.exe)
		else()
			set(PYTHONEXE python3)
		endif()
	endif()
	
	set(GENERATE_H_TESTS_FILE ${CMAKE_CURRENT_BINARY_DIR}/CGenerateTest.h)

	# Ensure output directory exists
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

	add_custom_command(
		OUTPUT ${GENERATE_H_TESTS_FILE}
		COMMAND ${PYTHONEXE} ${IMTCOREDIR}/Tests/Scripts/CGenerateTests.py ${pathToClassTest}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT
		"Generate tests for Partitura ImtCore")

	add_custom_target(
		GenereteTestHandler${PROJECT_NAME} ALL
		DEPENDS ${GENERATE_H_TESTS_FILE})

	add_dependencies(${PROJECT_NAME} GenereteTestHandler${PROJECT_NAME})

	target_sources(${PROJECT_NAME} PRIVATE ${GENERATE_H_TESTS_FILE})
endfunction(imtcore_generate_tests)

