project(imtcolorsdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlCustomConfig.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Lab.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/RgbColorHex.sdl
			)

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Lab.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/RgbColorHex.sdl"
			VERSION		"1.0")

