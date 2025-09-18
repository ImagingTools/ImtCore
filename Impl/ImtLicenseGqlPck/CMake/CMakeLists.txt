project(ImtLicenseGqlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)

target_link_libraries(${PROJECT_NAME} imtlicgql imtlic imtguigql imtgui imtwidgets imtauth imtdb imtservergql imtserverapp imtgql imtcol imtlicsdl imtbasesdl imtbase)

include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)
