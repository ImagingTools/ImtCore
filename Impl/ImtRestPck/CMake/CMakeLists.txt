project(ImtRestPck LANGUAGES CXX)

message("QT_DEFAULT_MAJOR_VERSION " ${QT_DEFAULT_MAJOR_VERSION})

find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS Core Network REQUIRED)

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtrest iproc)

include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)

target_link_libraries(${PROJECT_NAME} Qt5::Network )
