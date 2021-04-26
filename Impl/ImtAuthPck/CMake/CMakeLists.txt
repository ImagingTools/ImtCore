project(ImtAuthPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtauth)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)