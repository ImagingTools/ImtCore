project(imtdesign LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

find_package(Qt5 COMPONENTS Gui REQUIRED)
target_link_libraries(imtdesign Qt5::Gui)

find_package(Qt5 COMPONENTS Xml REQUIRED)
target_link_libraries(imtdesign Qt5::Xml)

