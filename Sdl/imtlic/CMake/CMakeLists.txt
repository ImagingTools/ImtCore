project(imtlicsdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlCustomConfig.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Features.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Licenses.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Products.sdl)

ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Features.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"Features"
			QML_NAME	"imtlicFeaturesSdl")
ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Licenses.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"Licenses"
			QML_NAME	"imtlicLicensesSdl")
ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Products.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"Products"
			QML_NAME	"imtlicProductsSdl")
