project(imtreport LANGUAGES CXX)

find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS PrintSupport REQUIRED)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} Qt5::PrintSupport)


