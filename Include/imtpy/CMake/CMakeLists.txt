project(imtpy LANGUAGES CXX)

if(NOT DEFINED PYTHON38DIR)
	set(PYTHON38DIR "$ENV{PYTHON38DIR}")
	if(PYTHON38DIR STREQUAL "")
		message(FATAL_ERROR "PYTHON38DIR environment variable is not set. Please set it to your Python 3.8 installation directory.")
	endif()
endif()

link_directories(${PYTHON38DIR}/libs)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/pybind11.cmake)