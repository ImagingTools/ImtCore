project(ImtCorePck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imeas iinsp iproc)

