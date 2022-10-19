project(imtej LANGUAGES CXX)

find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS PrintSupport REQUIRED)

add_compile_definitions(COMMON_LIBRARY)
include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/EjDoc.cmake)
include(${IMTCOREDIR}/Config/CMake/EjExport.cmake)
#include(${IMTCOREDIR}/Config/CMake/EjMiniZipper.cmake)


target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::PrintSupport)
