project(imtbasesdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlCustomConfig.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
			${CMAKE_CURRENT_LIST_DIR}/../1.0/ImtCollection.sdl)

ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/ComplexCollectionFilter.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"ComplexCollectionFilter"
			QML_NAME	"ComplexCollectionFilterSdl")

ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/ImtBaseTypes.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"ImtBaseTypes"
			QML_NAME	"ImtBaseTypesSdl")

ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/ImtCollection.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"ImtCollection"
			QML_NAME	"ImtCollectionSdl")
