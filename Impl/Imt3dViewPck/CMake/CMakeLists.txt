project(Imt3dViewPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} AcfSln::imeas AcfSln::iinsp AcfSln::iipr AcfSln::iproc imt3dview imt3d)
