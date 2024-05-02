cmake_minimum_required(VERSION 3.16)

function (imt_core_upgrade_partitura A_WORKING_DIRECTORY)

	set (CUSTOM_PYTHON OFF)
	if (NOT PYTHONEXE)
		set (CUSTOM_PYTHON ON)
		set(PYTHONEXE ${IMTCOREDIR}/3rdParty/Python/3.8/python.exe)

		if (NOT WIN32)
			set(PYTHONEXE python3)
		endif()
	endif()

	execute_process(
		COMMAND
			${PYTHONEXE} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../../PartituraUpgrade.py -D${A_WORKING_DIRECTORY}
		RESULT_VARIABLE
			PYTHONEXE_RESULT_CODE
		RESULTS_VARIABLE
			PYTHONEXE_RESULTS
		OUTPUT_VARIABLE
			PYTHONEXE_OUTPUT
		ERROR_VARIABLE
			PYTHONEXE_ERROR
	)

	if(NOT PYTHONEXE_RESULT_CODE EQUAL 0)
		message(WARNING "Unable to upgrade partitura versions")
		message(WARNING "${PYTHONEXE_RESULTS}")
		message(WARNING "${PYTHONEXE_OUTPUT}")
		message(WARNING "${PYTHONEXE_ERROR}")
		message(FATAL_ERROR "Process (${PYTHONEXE} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../../PartituraUpgrade.py) finished unexpacted! [${PYTHONEXE_RESULT_CODE}]\nPlease ensure, the path to the svn programm exists and available(add to PATH system variable)")
	endif()

	if (CUSTOM_PYTHON)
		unset(PYTHONEXE)
	endif()
	unset(CUSTOM_PYTHON)
endfunction()
