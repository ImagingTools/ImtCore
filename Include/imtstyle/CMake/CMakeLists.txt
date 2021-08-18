project(imtstyle LANGUAGES CXX)


include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)


find_package(Qt5 COMPONENTS Gui REQUIRED)
target_link_libraries(imtstyle Qt5::Gui)

