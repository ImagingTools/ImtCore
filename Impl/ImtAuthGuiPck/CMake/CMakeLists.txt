project(ImtAuthGuiPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtgui imtauth imtauthgui imtrepogui imtwidgets imtstyle imtqml)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
