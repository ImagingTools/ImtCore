project(imtappsdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlCustomConfig.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
			${CMAKE_CURRENT_LIST_DIR}/../1.0/ApplicationInfo.sdl)

ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/ApplicationInfo.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"Application"
			QML_NAME	"imtappApplicationSdl")

