project(ImtPyPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include({CMAKE_CURRENT_LIST_DIR}/../../../../Config/CMake/pybind11.cmake)

target_link_libraries(${PROJECT_NAME} imtpy)

