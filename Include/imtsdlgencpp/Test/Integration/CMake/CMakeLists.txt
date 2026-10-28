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
	"${TEST_DATA_DIR}/RequestedFields.sdl")

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

target_link_libraries(sdlgentestsdl PUBLIC
	imtbase
	imtgql
	imtservergql
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	sdlgentestsdl
	imtbase
	imtgql
	Acf::itest
	Qt${QT_VERSION_MAJOR}::Test
	Qt${QT_VERSION_MAJOR}::Core
)

deploy_qt(TARGET ${PROJECT_NAME})
