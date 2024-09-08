project(imtauthgql LANGUAGES CXX)	

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreProjectSdlConfig.cmake)

target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(${PROJECT_NAME} iauth imtgql imtbase)

ImtCoreConfigureSdlCppQml("RolesAPIv1_0" "CPP/Roles" "imtcore::sdl::Roles" "QML" "imtCoreRolesSdl")
ImtCoreConfigureSdlCppQml("UsersAPIv1_0" "CPP/Users" "imtcore::sdl::Users" "QML" "imtCoreUsersSdl")
ImtCoreConfigureSdlCppQml("GroupsAPIv1_0" "CPP/Groups" "imtcore::sdl::Groups" "QML" "imtCoreGroupsSdl")

