project(ImtEjPck LANGUAGES CXX)

add_compile_definitions(COMMON_LIBRARY)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/Quazip.cmake)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)

target_link_libraries(${PROJECT_NAME} imtej imtbase)

include(${IMTCOREDIR}/Config/CMake/EjMiniZipper.cmake)
include(${IMTCOREDIR}/Config/CMake/EjExport.cmake)
include(${IMTCOREDIR}/Config/CMake/EjDoc.cmake)

target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::PrintSupport)

include(${IACFDIR}/Config/CMake/zlib.cmake)
