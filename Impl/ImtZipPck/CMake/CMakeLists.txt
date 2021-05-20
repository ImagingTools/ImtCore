project(ImtZipPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTROOTDIR}/Config/CMake/Quazip.cmake)

target_link_libraries(${PROJECT_NAME} imtzip)

