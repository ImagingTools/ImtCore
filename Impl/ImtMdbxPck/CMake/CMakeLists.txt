project(ImtMdbxPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)

include(${IMTCOREDIR}/Config/CMake/Mdbx.cmake)

target_link_libraries(${PROJECT_NAME} imtmdbx)

