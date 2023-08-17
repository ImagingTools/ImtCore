project(ImtLicenseGuiPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} imeas iproc iauth iedge)
target_link_libraries(${PROJECT_NAME} imtgui imtloggui imtbase imtwidgets imtlic imtlicgui imtcrypt imtstyle)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)

