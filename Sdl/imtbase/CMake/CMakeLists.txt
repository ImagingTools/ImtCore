project(imtbasesdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlCustomConfig.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
			${CMAKE_CURRENT_LIST_DIR}/../1.0/ComplexCollectionFilter.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/ImtBaseTypes.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/ImtCollection.sdl
		)

ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/ImtBaseTypes.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"ImtBaseTypes"
			QML_NAME	"imtbaseImtBaseTypesSdl"
			FOUND_CPP_EXTRA_DEPS_OUT_VAR IMT_BASE_DEPS)

ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/ComplexCollectionFilter.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"ComplexCollectionFilter"
			QML_NAME	"imtbaseComplexCollectionFilterSdl"
			CPP_EXTRA_DEPS "${IMT_BASE_DEPS}"
			FOUND_CPP_EXTRA_DEPS_OUT_VAR COMPLEX_COLLECTION_FILTER_DEPS)

ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/ImtCollection.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"ImtCollection"
			QML_NAME	"imtbaseImtCollectionSdl"
			CPP_EXTRA_DEPS "${IMT_BASE_DEPS};${COMPLEX_COLLECTION_FILTER_DEPS}")
