project(ImtLicensePck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} imtbase imtgql imtlic)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

