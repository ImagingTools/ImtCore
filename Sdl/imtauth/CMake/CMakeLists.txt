project(imtauthsdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlAutoConfig.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Roles.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Users.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Groups.sdl)

ImtCoreAutoConfigureSdlCppQml(
			SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Roles.sdl"
			CUSTOM_TARGETS_DEPS ON
			CUSTOM_HEADER_DEP "1.0/CPP/Roles.h"
			CUSTOM_SOURCE_DEP "1.0/CPP/Roles.cpp"
			CUSTOM_QML_RES_DEP "1.0/QML/imtauthRolesSdl/imtauthRolesSdl.qrc")
ImtCoreAutoConfigureSdlCppQml(
			SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Users.sdl"
			CUSTOM_TARGETS_DEPS ON
			CUSTOM_HEADER_DEP "1.0/CPP/Users.h"
			CUSTOM_SOURCE_DEP "1.0/CPP/Users.cpp"
			CUSTOM_QML_RES_DEP "1.0/QML/imtauthUsersSdl/imtauthUsersSdl.qrc")
ImtCoreAutoConfigureSdlCppQml(
			SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Groups.sdl"
			CUSTOM_TARGETS_DEPS ON
			CUSTOM_HEADER_DEP "1.0/CPP/Groups.h"
			CUSTOM_SOURCE_DEP "1.0/CPP/Groups.cpp"
			CUSTOM_QML_RES_DEP "1.0/QML/imtauthGroupsSdl/imtauthGroupsSdl.qrc")

