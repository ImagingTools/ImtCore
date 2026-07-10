project(ImtFilePck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)


target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} imtfile imtbase)
target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} Acf::iqt Acf::i2d)

