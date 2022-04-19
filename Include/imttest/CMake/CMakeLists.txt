project(imttest)

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(${PROJECT_NAME} itest)
