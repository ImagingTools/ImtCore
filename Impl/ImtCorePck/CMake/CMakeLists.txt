project(ImtCorePck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Qml)

target_link_libraries(${PROJECT_NAME} imtbase imeas iinsp iproc)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Qml)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
