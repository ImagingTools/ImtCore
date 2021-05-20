project(ImtReportPck LANGUAGES CXX)

find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS PrintSupport REQUIRED)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} imtreport imtbase)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)

target_link_libraries( Qt${QT_DEFAULT_MAJOR_VERSION}::PrintSupport)
