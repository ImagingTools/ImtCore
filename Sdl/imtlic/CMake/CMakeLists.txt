project(imtlicsdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlAutoConfig.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Features.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Licenses.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Products.sdl)

ImtCoreAutoConfigureSdlCppQml(
			SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Features.sdl"
			CUSTOM_TARGETS_DEPS ON
			CUSTOM_HEADER_DEP "1.0/CPP/Features.h"
			CUSTOM_SOURCE_DEP "1.0/CPP/Features.cpp"
			CUSTOM_QML_RES_DEP "1.0/QML/imtlicFeaturesSdl/imtlicFeaturesSdl.qrc")
ImtCoreAutoConfigureSdlCppQml(
			SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Licenses.sdl"
			CUSTOM_TARGETS_DEPS ON
			CUSTOM_HEADER_DEP "1.0/CPP/Licenses.h"
			CUSTOM_SOURCE_DEP "1.0/CPP/Licenses.cpp"
			CUSTOM_QML_RES_DEP "1.0/QML/imtlicLicensesSdl/imtlicLicensesSdl.qrc")
ImtCoreAutoConfigureSdlCppQml(
			SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Products.sdl"
			CUSTOM_TARGETS_DEPS ON
			CUSTOM_HEADER_DEP "1.0/CPP/Products.h"
			CUSTOM_SOURCE_DEP "1.0/CPP/Products.cpp"
			CUSTOM_QML_RES_DEP "1.0/QML/imtlicProductsSdl/imtlicProductsSdl.qrc")
