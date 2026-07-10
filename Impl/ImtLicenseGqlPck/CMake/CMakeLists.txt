project(ImtLicenseGqlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} imtlicgql imtlic imtguigql imtgui imtwidgets imtauth imtdb imtservergql imtserverapp imtgql imtcol imtlicsdl imtbasesdl imtbase)

