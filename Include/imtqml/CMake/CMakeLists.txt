project(imtqml LANGUAGES CXX)

find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS QuickWidgets REQUIRED)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${ACFSLNDIR}/Config/CMake/AcfSln.cmake)

target_link_libraries(${PROJECT_NAME} Qt5::QuickWidgets)
