project(imtrest)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/Quazip.cmake)

target_link_libraries(${PROJECT_NAME} imod iser imtbase)

target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::WebSockets)
