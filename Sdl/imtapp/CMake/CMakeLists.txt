project(imtappsdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlAutoConfig.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
			${CMAKE_CURRENT_LIST_DIR}/../1.0/ApplicationInfo.sdl)

ImtCoreAutoConfigureSdlCppQml(
			SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/ApplicationInfo.sdl"
			CUSTOM_TARGETS_DEPS ON
			CUSTOM_HEADER_DEP "1.0/CPP/Application.h"
			CUSTOM_SOURCE_DEP "1.0/CPP/Application.cpp"
			CUSTOM_QML_RES_DEP "1.0/QML/imtappApplicationSdl/imtappApplicationSdl.qrc")

