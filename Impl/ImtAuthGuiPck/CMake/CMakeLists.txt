project(ImtAuthGuiPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtgui imtauth imtauthgui imtwidgets imtstyle imtqml imtgql)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
