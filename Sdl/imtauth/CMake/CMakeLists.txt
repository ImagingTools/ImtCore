project(imtauthsdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlCustomConfig.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Roles.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Users.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Groups.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Authorization.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Profile.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Sessions.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/AuthorizationOptions.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/UserActions.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/PersonalAccessTokens.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Tenants.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/TenantCollectionDocumentService.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/RelationshipCollectionDocumentService.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/TenantMemberships.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/RoleCollectionDocumentService.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/GroupCollectionDocumentService.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/UserCollectionDocumentService.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/CrossOrgGrantCollectionDocumentService.sdl
			)

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Roles.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Users.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Groups.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Authorization.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Profile.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Sessions.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/AuthorizationOptions.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/UserActions.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/PersonalAccessTokens.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Tenants.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/TenantCollectionDocumentService.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/RelationshipCollectionDocumentService.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/TenantMemberships.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/RoleCollectionDocumentService.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/GroupCollectionDocumentService.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/UserCollectionDocumentService.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/CrossOrgGrantCollectionDocumentService.sdl"
			VERSION		"1.0")
