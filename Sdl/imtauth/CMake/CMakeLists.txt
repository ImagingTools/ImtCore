project(imtauthsdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreProjectSdlConfig.cmake)

# target_link_libraries(${PROJECT_NAME} imtbase)

ImtCoreConfigureSdlCppQml("imtauth/1.0/Roles" "CPP/Roles" "sdl::imtauth::Roles" "QML" "imtauthRolesSdl")
ImtCoreConfigureSdlCppQml("imtauth/1.0/Users" "CPP/Users" "sdl::imtauth::Users" "QML" "imtauthUsersSdl")
ImtCoreConfigureSdlCppQml("imtauth/1.0/Groups" "CPP/Groups" "sdl::imtauth::Groups" "QML" "imtauthGroupsSdl")

