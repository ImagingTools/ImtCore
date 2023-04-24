project(ImtMongoPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

include(${IMTCOREDIR}/Config/CMake/MongoCXX.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtdb imtmongo)

