include(${ACFDIR}/Config/CMake/AcfStd.cmake)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} advapi32 user32)
endif()

function(imtcore_generate_tests pathToClassTest)
	set(PYTHONEXE ${IMTCOREDIR}/3rdParty/Python/3.8/python.exe)
	set(GENERATE_H_TESTS_FILE ${CMAKE_CURRENT_SOURCE_DIR}../CGenerateTest.h)
	if (NOT WIN32 AND NOT ANDROID)
		set(PYTHONEXE python3)
	endif()

	add_custom_command(
		OUTPUT ${GENERATE_H_TESTS_FILE}
		PRE_BUILD
		COMMAND ${PYTHONEXE} ${IMTCOREDIR}/Tests/Scripts/CGenerateTests.py ${pathToClassTest}
		COMMENT
		"Generate tests for Partitura ImtCore")

	add_custom_target(
		GenereteTestHandler${PROJECT_NAME} ALL
		DEPENDS ${GENERATE_H_TESTS_FILE})

	target_sources(${PROJECT_NAME} PRIVATE ${GENERATE_H_TESTS_FILE})
endfunction(imtcore_generate_tests)

