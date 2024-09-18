project(imtlicsdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlAutoConfig.cmake)


set(SDL_SOURCES)
list(APPEND SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Features.sdl")
list(APPEND SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Licenses.sdl")
list(APPEND SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../1.0/Products.sdl")
ImtCoreAutoConfigureSdlCppQml("${SDL_SOURCES}")
