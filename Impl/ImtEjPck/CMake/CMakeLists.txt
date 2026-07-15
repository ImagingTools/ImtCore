project(ImtEjPck LANGUAGES CXX)


add_compile_definitions(COMMON_LIBRARY)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/Quazip.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE}
	imtej
)

include(${IMTCOREDIR}/Config/CMake/EjMiniZipper.cmake)
include(${IMTCOREDIR}/Config/CMake/EjExport.cmake)
include(${IMTCOREDIR}/Config/CMake/EjDoc.cmake)
include(${IACFDIR}/Config/CMake/zlib.cmake)

