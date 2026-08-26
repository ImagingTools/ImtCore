project(imt3dTest)


include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imt3d
	Acf::itest
	Acf::istd
	Acf::imath
	Acf::iser
	Acf::iimg
	Acf::idoc
	Acf::i2d
	Qt${QT_VERSION_MAJOR}::Test
	Qt${QT_VERSION_MAJOR}::Core
)

deploy_qt(TARGET ${PROJECT_NAME})

