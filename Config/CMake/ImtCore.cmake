include(${ACFDIR}/Config/CMake/AcfStd.cmake)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} advapi32 user32)
endif()

function(imtcore_generate_tests)
        set(PYTHONEXE ${IMTCOREDIR}/3rdParty/Python/3.8/python.exe)
	set(GENERATE_H_TESTS_FILE ${IMTCOREDIR}/Tests/ObjectCollectionTest/CGenerateObjectCollectionTest.h)
	set(PARTITURA_DIR ${IMTCOREDIR}/Tests/ObjectCollectionTest/Partitura)

	if (NOT WIN32)
	        set(PYTHONEXE python3)
	endif()

	add_custom_command(
	        OUTPUT ${GENERATE_H_TESTS_FILE}
		PRE_BUILD
		COMMAND ${PYTHONEXE} ${IMTCOREDIR}/Tests/ObjectCollectionTest/CGenerateObjectCollectionTest.py ${PARTITURA_DIR}
		COMMENT
		"Generate tests for ImtCore"
	        )

	add_custom_target(
	        GenereteTestHandler${PROJECT_NAME} ALL
		DEPENDS ${GENERATE_H_TESTS_FILE}
	        )

endfunction(imtcore_generate_tests)
