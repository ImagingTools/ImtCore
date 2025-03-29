project(ImtFilePck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)


target_link_libraries(${PROJECT_NAME} imtfile imtbase)
target_link_libraries(${PROJECT_NAME} iqt i2d)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
