project(imtpy LANGUAGES CXX)

link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../3rdParty/Python/3.8/libs)

include($ENV{ACFCONFIGDIR}/CMake/StaticConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)
include(../../../Config/CMake/pybind11.cmake)