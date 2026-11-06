project(ImtSdlGenIntegrationTest)


if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "off")
endif()

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlCustomConfig.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

set(TEST_DATA_DIR "${PROJECT_SOURCE_DIR}/../../TestData")

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
	"${TEST_DATA_DIR}/ArraySerialization.sdl"
	"${TEST_DATA_DIR}/RequestedFields.sdl"
	"${TEST_DATA_DIR}/NoObjects.sdl"
	"${TEST_DATA_DIR}/NoObjectsConsumer.sdl")

# The library with the code, generated from the test schemas
add_library(sdlgentestsdl STATIC)

block()
	set(PROJECT_NAME sdlgentestsdl)
	ImtCoreCustomConfigureSdlCpp(
		SCHEMA_PATH "${TEST_DATA_DIR}/ArraySerialization.sdl"
		VERSION "1.0")
endblock()

block()
	set(PROJECT_NAME sdlgentestsdl)
	ImtCoreCustomConfigureSdlCpp(
		SCHEMA_PATH "${TEST_DATA_DIR}/RequestedFields.sdl"
		VERSION "1.0")
endblock()

add_dependencies(sdlgentestsdl SdlCodeGenerator)

# The library with the code, generated WITHOUT the QObject based classes ('<Schema>Objects' files, see NO_OBJECTS).
# The schemas of the library import each other, so all of them are generated without the objects
add_library(sdlgentestnoobjectssdl STATIC)

block()
	set(PROJECT_NAME sdlgentestnoobjectssdl)
	ImtCoreCustomConfigureSdlCpp(NO_OBJECTS
		SCHEMA_PATH "${TEST_DATA_DIR}/NoObjects.sdl"
		VERSION "1.0")
endblock()

block()
	set(PROJECT_NAME sdlgentestnoobjectssdl)
	ImtCoreCustomConfigureSdlCpp(NO_OBJECTS
		SCHEMA_PATH "${TEST_DATA_DIR}/NoObjectsConsumer.sdl"
		VERSION "1.0")
endblock()

add_dependencies(sdlgentestnoobjectssdl SdlCodeGenerator)

target_link_libraries(sdlgentestnoobjectssdl PUBLIC
	imtbase
	imtgql
	imtservergql
)

target_link_libraries(sdlgentestsdl PUBLIC
	imtbase
	imtgql
	imtservergql
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	sdlgentestsdl
	sdlgentestnoobjectssdl
	imtbase
	imtgql
	Acf::itest
	Qt${QT_VERSION_MAJOR}::Test
	Qt${QT_VERSION_MAJOR}::Core
)

deploy_qt(TARGET ${PROJECT_NAME})


# The tests of the SDL CMake macros (NO_OBJECTS, the collisions of the outputs).
# They are only configure the small projects. Run: ctest --test-dir <build directory of this project>
find_program(IMT_SDL_MACRO_TEST_NINJA ninja)
if (IMT_SDL_MACRO_TEST_NINJA)
	set(IMT_SDL_MACRO_TEST_QT_ARGS)
	if (DEFINED Qt${QT_VERSION_MAJOR}_DIR)
		list(APPEND IMT_SDL_MACRO_TEST_QT_ARGS -DQt${QT_VERSION_MAJOR}_DIR=${Qt${QT_VERSION_MAJOR}_DIR})
	endif()

	enable_testing()
	add_test(
		NAME ImtSdlCMakeMacros
		COMMAND ${CMAKE_COMMAND}
			-DIMTCOREDIR=${IMTCOREDIR}
			${IMT_SDL_MACRO_TEST_QT_ARGS}
			-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/MacroTests
			-DTEST_GENERATOR=Ninja
			-P ${CMAKE_CURRENT_LIST_DIR}/MacroTest/RunMacroTests.cmake)
else()
	message(STATUS "ninja is not found: the SDL CMake macro tests are skipped")
endif()
