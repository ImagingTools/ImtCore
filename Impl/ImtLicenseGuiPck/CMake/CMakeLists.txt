project(ImtLicenseGuiPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} imtlicgui imtlic imtgui imtloggui imtbase imtwidgets imtcrypt imtstyle)
target_link_libraries(${PROJECT_NAME} imeas iproc iauth iedge)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)

