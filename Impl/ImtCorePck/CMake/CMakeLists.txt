project(ImtCorePck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} imtbase AcfSln::imeas AcfSln::iinsp AcfSln::iproc AcfSln::icomm)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
