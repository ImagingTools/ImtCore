project(Imt3dPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} imtbase imt3d AcfSln::imeas AcfSln::iinsp AcfSln::iproc AcfSln::imeas AcfSln::iinsp AcfSln::iproc AcfSln::icalib)

