project(ImtPyPck LANGUAGES CXX)

link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../3rdParty/Python/3.8/libs)

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)
include(../../../Config/CMake/pybind11.cmake)

include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)


target_link_libraries(${PROJECT_NAME} imtpy)