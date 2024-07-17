project(ImtReportPck LANGUAGES CXX)

add_compile_definitions(COMMON_LIBRARY)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)

target_link_libraries(${PROJECT_NAME} imtreport imtbase)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)
target_link_libraries(${PROJECT_NAME} imath icmm)

target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::PrintSupport)
