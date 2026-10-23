project(ImtRestDemoClient LANGUAGES CXX)


if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "off")
endif()

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtrestdemo
	Qt${QT_VERSION_MAJOR}::Gui
	Qt${QT_VERSION_MAJOR}::Qml
	Qt${QT_VERSION_MAJOR}::Quick
	Qt${QT_VERSION_MAJOR}::QuickControls2
)

deploy_qt(TARGET ${PROJECT_NAME})
