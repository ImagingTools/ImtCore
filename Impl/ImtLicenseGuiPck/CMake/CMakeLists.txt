project(ImtLicenseGuiPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)

target_link_libraries(${PROJECT_NAME} imeas iproc iauth iedge)
target_link_libraries(${PROJECT_NAME} imtgui imtloggui imtbase imtwidgets imtlic imtlicgui imtrepogui imtcrypt imtstyle)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
