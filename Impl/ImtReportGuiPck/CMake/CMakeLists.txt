project(ImtReportGuiPck LANGUAGES CXX)

find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS PrintSupport REQUIRED)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFSLNDIR}/Config/CMake/AcfSln.cmake)

target_link_libraries(${PROJECT_NAME} imtreport imtreportgui)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)

target_link_libraries(${PROJECT_NAME} Qt5::PrintSupport)