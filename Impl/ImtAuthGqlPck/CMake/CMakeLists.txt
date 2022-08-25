project(ImtAuthGqlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} imtauthgql imtauth imtlic imtguigql imtbase imtgql imtgui imtwidgets)


include(${ACFDIR}/Config/CMake/AcfQt.cmake)
