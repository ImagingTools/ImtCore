project(ImtSdlGenTest)


set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../../Config/BaseOnly.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")
set(ARX_ENABLE_GENERATE_DEPENDENCIES_LIST OFF)

if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "off")
endif()

include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlCustomConfig.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/../TestData/ArraySerialization.sdl")

add_library(sdlgentestsdl STATIC)

block()
	set(PROJECT_NAME sdlgentestsdl)
	unset(MOC_SOURCES)
	ImtCoreCustomConfigureSdlCpp(
		SCHEMA_PATH "${CMAKE_CURRENT_LIST_DIR}/../TestData/ArraySerialization.sdl"
		VERSION "1.0")
endblock()

add_dependencies(sdlgentestsdl SdlCodeGenerator)

target_link_libraries(sdlgentestsdl PUBLIC
	imtservergql
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtsdlgencpp
	imtsdlgenqml
	sdlgentestsdl
	Acf::itest
	Qt${QT_VERSION_MAJOR}::Test
)

deploy_qt(TARGET ${PROJECT_NAME})

