if(NOT DEFINED PYTHON38DIR)
	set(PYTHON38DIR "$ENV{PYTHON38DIR}")
	if(PYTHON38DIR STREQUAL "")
		message(FATAL_ERROR "PYTHON38DIR environment variable is not set. Please set it to your Python 3.8 installation directory.")
	endif()
endif()

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../3rdParty/pybind11/include")
include_directories("${PYTHON38DIR}/include")
link_directories("${PYTHON38DIR}/libs")

target_link_libraries(${PROJECT_NAME} ${PYTHON38DIR}/libs/python38.lib)

