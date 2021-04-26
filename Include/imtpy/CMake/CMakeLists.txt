project(imtpy LANGUAGES CXX)

link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../3rdParty/Python/3.8/libs)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${IMT_DIR}/Config/CMake/pybind11.cmake)