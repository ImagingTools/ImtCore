project(ImtLogPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imeas iinsp iproc imtlog iauth)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)