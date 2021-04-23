project(imtreport LANGUAGES CXX)

find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS PrintSupport REQUIRED)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${ACFSLNDIR}/Config/CMake/AcfSln.cmake)

target_link_libraries(Qt5::PrintSupport)
