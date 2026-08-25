project(imt3dTest)


include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imt3d
	AcfSln::icalib
	Acf::itest
	Acf::istd
	Acf::imath
	Acf::i3d
	Acf::iser
	Acf::iimg
	Acf::idoc
	Acf::i2d
	Qt${QT_VERSION_MAJOR}::Test
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Gui
	Qt${QT_VERSION_MAJOR}::Svg
)

deploy_qt(TARGET ${PROJECT_NAME})

