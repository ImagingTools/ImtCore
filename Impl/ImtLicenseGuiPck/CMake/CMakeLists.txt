project(ImtLicenseGuiPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} imtlicgui imtlic imtgui imtloggui imtbase imtwidgets imtcrypt imtstyle)
target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} AcfSln::imeas AcfSln::iproc AcfSln::iauth AcfSln::iedge)

