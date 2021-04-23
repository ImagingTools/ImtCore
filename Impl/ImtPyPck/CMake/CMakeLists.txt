project(ImtPyPck LANGUAGES CXX)

link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../3rdParty/Python/3.8/libs)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFSLNDIR}/Config/CMake/AcfSln.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${IMT_DIR}/Config/CMake/pybind11.cmake)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)

target_link_libraries(${PROJECT_NAME} imtpy)