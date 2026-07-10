project(ImtLogPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} imtlog imtbase AcfSln::iauth)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
