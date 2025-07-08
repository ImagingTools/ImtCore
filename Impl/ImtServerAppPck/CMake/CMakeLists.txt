project(ImtServerAppPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Qml)
target_link_libraries(${PROJECT_NAME} imtbasesdl imtbase imtgql imtserverapp)

include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)


