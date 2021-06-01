project(ImtRepoGuiPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtgui imtwidgets imtrepo imtrepogui)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
