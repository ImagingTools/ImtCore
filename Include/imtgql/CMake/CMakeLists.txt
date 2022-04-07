project(imtgql)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

qt5_use_modules(${PROJECT_NAME} WebSockets)
