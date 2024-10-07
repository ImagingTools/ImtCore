project(imtauthsdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlCustomConfig.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Roles.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Users.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Groups.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Authorization.sdl)

ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Roles.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"Roles"
			QML_NAME	"imtauthRolesSdl")
ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Users.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"Users"
			QML_NAME	"imtauthUsersSdl")
ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Groups.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"Groups"
			QML_NAME	"imtauthGroupsSdl")
ImtCoreCustomConfigureSdlCppQml(
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Authorization.sdl"
			VERSION		"1.0"
			SOURCE_NAME	"Authorization"
			QML_NAME	"imtauthAuthorizationSdl")

