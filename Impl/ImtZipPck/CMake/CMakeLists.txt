project(ImtZipPck LANGUAGES CXX)

#include_directories($ENV{QUAZIP}/include)
#include_directories($ENV{ZLIBDIR}/include)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
#include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../../../Config/CMake/Quazip.cmake)

target_link_libraries(${PROJECT_NAME} C:/Work/Rpos/ITDevelopment/ImtCore/Lib/DebugVC15_64/imtzip.lib)
#target_link_libraries(${PROJECT_NAME} imtzip)