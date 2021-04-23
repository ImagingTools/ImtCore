project(ImtRestPck LANGUAGES CXX)

message("QT_DEFAULT_MAJOR_VERSION " ${QT_DEFAULT_MAJOR_VERSION})

find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS Core Network REQUIRED)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFSLNDIR}/Config/CMake/AcfSln.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtrest iproc)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)

target_link_libraries(${PROJECT_NAME} Qt5::Network )
