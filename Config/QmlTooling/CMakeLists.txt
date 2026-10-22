cmake_minimum_required(VERSION 3.22)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	project(ImtCoreQmlTooling LANGUAGES NONE)
endif()

if(TARGET qml_tooling_metadata)
	return()
endif()

get_filename_component(_imt_qml_source_root "../.." ABSOLUTE BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")

set(_imt_qml_qt_root "${QT_HOST_PATH}")
if(NOT _imt_qml_qt_root AND TARGET Qt6::qmake)
	get_target_property(_imt_qml_qmake Qt6::qmake IMPORTED_LOCATION)
	if(_imt_qml_qmake)
		get_filename_component(_imt_qml_qt_bin "${_imt_qml_qmake}" DIRECTORY)
		get_filename_component(_imt_qml_qt_root "${_imt_qml_qt_bin}/.." ABSOLUTE)
	endif()
endif()

set(_imt_qml_python "${PYTHON_EXECUTABLE}")
if(NOT _imt_qml_python)
	unset(_imt_qml_python)
	find_program(_imt_qml_python NAMES python3 python)
endif()
if(NOT _imt_qml_python)
	set(_imt_qml_python python3)
endif()

set(IMT_QML_TOOLING_QT_ROOT "${_imt_qml_qt_root}" CACHE PATH "Host Qt 6 installation used only for editor metadata")
set(IMT_QML_TOOLING_PYTHON "${_imt_qml_python}" CACHE FILEPATH "Python interpreter used only for editor metadata")
set(IMT_QML_TOOLING_OUTPUT "${CMAKE_BINARY_DIR}/qml-tooling" CACHE PATH "Editor-only QML import root; never deploy this directory")

add_custom_target(qml_tooling_metadata
	COMMAND "${IMT_QML_TOOLING_PYTHON}" "${CMAKE_CURRENT_LIST_DIR}/generate.py"
		--source-root "${_imt_qml_source_root}"
		--manifest "${CMAKE_CURRENT_LIST_DIR}/modules.json"
		--qt-root "${IMT_QML_TOOLING_QT_ROOT}"
		--output "${IMT_QML_TOOLING_OUTPUT}"
	SOURCES "${CMAKE_CURRENT_LIST_DIR}/generate.py" "${CMAKE_CURRENT_LIST_DIR}/modules.json"
	COMMENT "Generating editor-only QML metadata (no C++ compilation)"
	VERBATIM
	USES_TERMINAL
)
set_target_properties(qml_tooling_metadata PROPERTIES FOLDER "Tooling")
