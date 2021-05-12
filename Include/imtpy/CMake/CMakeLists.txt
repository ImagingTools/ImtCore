project(imtpy LANGUAGES CXX)

link_directories(${PYTHON38DIR}/libs)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${IMTCOREDIR}/Config/CMake/pybind11.cmake)