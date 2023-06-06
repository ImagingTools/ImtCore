project(imtgeo LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Positioning REQUIRED)
find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Location REQUIRED)

target_link_libraries(${PROJECT_NAME} Qt${QT_DEFAULT_MAJOR_VERSION}::Positioning)
target_link_libraries(${PROJECT_NAME} Qt${QT_DEFAULT_MAJOR_VERSION}::Location)
