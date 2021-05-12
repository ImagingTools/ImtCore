project(ImtZipPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include($ENV{IMTCOREDIR}/Config/CMake/Quazip.cmake)

target_link_libraries(${PROJECT_NAME} imtzip)