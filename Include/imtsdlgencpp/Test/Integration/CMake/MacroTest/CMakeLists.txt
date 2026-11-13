# The project to check the SDL CMake macros (see 'RunMacroTests.cmake').
# It is only configured, the generator is not started: the tests check the registered sources and commands.
cmake_minimum_required(VERSION 3.26)

project(sdlmacrotest LANGUAGES CXX)


if(NOT DEFINED IMTCOREDIR)
	message(FATAL_ERROR "IMTCOREDIR is not set")
endif()

find_package(Qt6 COMPONENTS Core REQUIRED)

set(AUX_INCLUDE_DIR "${CMAKE_BINARY_DIR}/GeneratedFiles")
set(TEST_DATA_DIR "${IMTCOREDIR}/Include/imtsdlgencpp/Test/TestData")

include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlCustomConfig.cmake)

# the generator is not started, so any path is acceptable
function(GetSdlGeneratorPath OUTPUT_SDL_GENERATOR_EXE_PATH)
	set(${OUTPUT_SDL_GENERATOR_EXE_PATH} "${CMAKE_BINARY_DIR}/SdlCodeGenerator" PARENT_SCOPE)
endfunction()

add_library(${PROJECT_NAME} STATIC)


#! Checks the count of the project sources, matched with the \c REGEX
function(expect_sources_count REGEX EXPECTED_COUNT)
	get_target_property(PROJECT_SOURCES ${PROJECT_NAME} SOURCES)
	list(FILTER PROJECT_SOURCES INCLUDE REGEX "${REGEX}")
	list(LENGTH PROJECT_SOURCES ACTUAL_COUNT)
	if (NOT ACTUAL_COUNT EQUAL EXPECTED_COUNT)
		message(FATAL_ERROR "Sources matched '${REGEX}': expected ${EXPECTED_COUNT}, actual ${ACTUAL_COUNT}: ${PROJECT_SOURCES}")
	endif()
endfunction()


if (TEST_CASE STREQUAL "cxx_objects")
	# default: the objects are generated
	ImtCoreCustomConfigureSdlCppQml(CXX
		SCHEMA_PATH "${TEST_DATA_DIR}/ArraySerialization.sdl"
		VERSION "1.0")

	expect_sources_count("/ArraySerialization\\.h$" 1)
	expect_sources_count("/ArraySerialization\\.cpp$" 1)
	expect_sources_count("/ArraySerializationObjects\\.h$" 1)
	expect_sources_count("/moc_ArraySerialization\\.cpp$" 1)
	expect_sources_count("/moc_ArraySerializationObjects\\.cpp$" 1)

elseif (TEST_CASE STREQUAL "cxx_no_objects")
	# NO_OBJECTS is forwarded by the 'CppQml' macro
	ImtCoreCustomConfigureSdlCppQml(CXX NO_OBJECTS
		SCHEMA_PATH "${TEST_DATA_DIR}/ArraySerialization.sdl"
		VERSION "1.0")

	expect_sources_count("/ArraySerialization\\.h$" 1)
	expect_sources_count("/ArraySerialization\\.cpp$" 1)
	expect_sources_count("/moc_ArraySerialization\\.cpp$" 1)
	expect_sources_count("Objects" 0)

elseif (TEST_CASE STREQUAL "cpp_no_objects")
	# NO_OBJECTS of the 'Cpp' macro
	ImtCoreCustomConfigureSdlCpp(NO_OBJECTS
		SCHEMA_PATH "${TEST_DATA_DIR}/ArraySerialization.sdl"
		VERSION "1.0")

	expect_sources_count("/ArraySerialization\\.h$" 1)
	expect_sources_count("Objects" 0)

elseif (TEST_CASE STREQUAL "independent_flags")
	# the NO_OBJECTS flag belongs to a schema: the schemas configured after it are not affected
	ImtCoreCustomConfigureSdlCppQml(CXX NO_OBJECTS
		SCHEMA_PATH "${TEST_DATA_DIR}/ArraySerialization.sdl"
		VERSION "1.0")
	ImtCoreCustomConfigureSdlCppQml(CXX
		SCHEMA_PATH "${TEST_DATA_DIR}/RequestedFields.sdl"
		VERSION "1.0")

	# the second schema has the objects
	expect_sources_count("/ArraySerializationObjects\\.h$" 0)
	expect_sources_count("/RequestedFieldsObjects\\.h$" 1)

elseif (TEST_CASE STREQUAL "qml_no_objects")
	# the QML types are generated independently of the C++ objects: the C++ part has no objects, the QML part is generated
	ImtCoreCustomConfigureSdlCppQml(CXX QML NO_OBJECTS
		SCHEMA_PATH "${TEST_DATA_DIR}/ArraySerialization.sdl"
		VERSION "1.0")

	expect_sources_count("/ArraySerialization\\.h$" 1)
	expect_sources_count("/ArraySerialization\\.cpp$" 1)
	expect_sources_count("Objects" 0)

elseif (TEST_CASE STREQUAL "collision")
	# the same schema twice: the outputs are the same
	ImtCoreCustomConfigureSdlCppQml(CXX
		SCHEMA_PATH "${TEST_DATA_DIR}/ArraySerialization.sdl"
		VERSION "1.0")
	ImtCoreCustomConfigureSdlCppQml(CXX
		SCHEMA_PATH "${TEST_DATA_DIR}/ArraySerialization.sdl"
		VERSION "1.0")

else()
	message(FATAL_ERROR "Unknown TEST_CASE '${TEST_CASE}'")
endif()
