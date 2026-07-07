project(ImtNotifyDbPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtdb imtnotify imtnotifydb imtcol)

include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)
