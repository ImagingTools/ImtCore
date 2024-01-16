project(imtgqlrepo)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS WebSockets REQUIRED)
target_link_libraries(${PROJECT_NAME} imtgql imtbase Qt${QT_VERSION_MAJOR}::WebSockets)
