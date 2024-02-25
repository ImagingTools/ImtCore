cmake_minimum_required(VERSION 3.1.0)


if(NOT DEFINED IACFDIR)
	set(IACFDIR "$ENV{IACFDIR}")
endif()

include(${IACFDIR}/Config/CMake/IAcfEnv.cmake)

if(NOT DEFINED IMTCOREDIR)
	set(IMTCOREDIR "$ENV{IMTCOREDIR}")
endif()

if(NOT DEFINED OPENSSLDIR)
    set(OPENSSLDIR "$ENV{OPENSSLDIR}")
    if(OPENSSLDIR STREQUAL "")
        set(OPENSSLDIR "${IMTCOREDIR}/3rdParty/openssl/1.1")
    endif()
endif()

if(NOT DEFINED QUAZIP)
	set(QUAZIP "${IMTCOREDIR}/3rdParty/quazip")
endif()
if(NOT DEFINED PYTHON38DIR)
	set(PYTHON38DIR "${IMTCOREDIR}/3rdParty/Python/3.8")
endif()

if(NOT DEFINED ACFPUBLICDIR)
	set(ACFPUBLICDIR "$ENV{ACFPUBLICDIR}")
endif()

if(NOT DEFINED ZLIBDIR)
	set(ZLIBDIR "$ENV{ZLIBDIR}")
	if(ZLIBDIR STREQUAL "")
		set(ZLIBDIR "${ACFPUBLICDIR}/3rdParty/zlib")
	endif()
endif()

include_directories("${IMTCOREDIR}/AuxInclude/${TARGETNAME}")
include_directories("${IMTCOREDIR}/Include")
include_directories("${IMTCOREDIR}/Impl")

link_directories("${IMTCOREDIR}/Lib/${CMAKE_BUILD_TYPE}_${TARGETNAME}")

# upgrade versions
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
		${PYTHONEXE} ${CMAKE_CURRENT_LIST_DIR}/../../../PartituraUpgrade.py
	WORKING_DIRECTORY
		${CMAKE_CURRENT_LIST_DIR}/../../..
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
	message(FATAL_ERROR "Process (${PYTHONEXE} ${CMAKE_CURRENT_LIST_DIR}/../../../PartituraUpgrade.py) finished unexpacted! [${PYTHONEXE_RESULT_CODE}]")
endif()

if (CUSTOM_PYTHON)
	unset(PYTHONEXE)
	unset(CUSTOM_PYTHON)
endif()



