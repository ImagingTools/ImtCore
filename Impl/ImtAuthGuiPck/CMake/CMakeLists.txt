project(ImtAuthGuiPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFSLNDIR}/Config/CMake/AcfSln.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtgui imtauth imtauthgui imtrepogui imtwidgets)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)