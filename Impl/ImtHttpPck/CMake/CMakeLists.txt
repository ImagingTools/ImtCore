project(ImtHttpPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtcom imthttp)


