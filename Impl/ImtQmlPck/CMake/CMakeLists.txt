project(ImtQmlPck LANGUAGES CXX)

find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS QuickWidgets REQUIRED)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFSLNDIR}/Config/CMake/AcfSln.cmake)

target_link_libraries(${PROJECT_NAME} Qt5::QuickWidgets)
target_link_libraries(${PROJECT_NAME} imtqml)

IF(WIN32)
	target_link_libraries(${PROJECT_NAME} User32 Advapi32)
ENDIF()

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)