project(ImtPyPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include({CMAKE_CURRENT_LIST_DIR}/../../../../Config/CMake/pybind11.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} imtpy)

