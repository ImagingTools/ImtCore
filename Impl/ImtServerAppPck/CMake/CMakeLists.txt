project(ImtServerAppPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} imtserverapp imtbasesdl imtservergql imtgql imtrest imtcom imtdb imtlic imtbase)

include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)


