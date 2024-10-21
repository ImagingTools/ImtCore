project(imtcollectionsdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlCustomConfig.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
			${CMAKE_CURRENT_LIST_DIR}/../1.0/ComplexCollectionFilter.sdl)

ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/ComplexCollectionFilter.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"ComplexCollectionFilter"
			QML_NAME	"complexCollectionFilterSdl")
