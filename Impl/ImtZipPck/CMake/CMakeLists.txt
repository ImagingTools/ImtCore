project(ImtZipPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include({CMAKE_CURRENT_LIST_DIR}/../../../../Config/CMake/Quazip.cmake)

target_link_libraries(${PROJECT_NAME} imtzip)

