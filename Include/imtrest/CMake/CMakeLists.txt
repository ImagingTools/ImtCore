project(imtrest)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${ACFSLNDIR}/Config/CMake/AcfSln.cmake)

find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS Core Network REQUIRED)