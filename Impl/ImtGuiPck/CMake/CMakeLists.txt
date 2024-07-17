project(ImtGuiPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} imeas iproc iedge iqtdoc)
target_link_libraries(${PROJECT_NAME} imtgui iauth imtloggui imtbase imtwidgets i2d iqt2d imtstyle)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)

