project(imtauthsdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlAutoConfig.cmake)


set(SDL_SOURCES)
list(APPEND SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Roles.sdl")
list(APPEND SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Users.sdl")
list(APPEND SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Groups.sdl")
ImtCoreAutoConfigureSdlCppQml("${SDL_SOURCES}")

