project(imtqml LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 11)

find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS QuickWidgets REQUIRED)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::QuickWidgets)
target_link_libraries(${PROJECT_NAME} iauth)
