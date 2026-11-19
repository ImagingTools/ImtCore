project(imtrestTest)


include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtrest
	imtcom
	Acf::itest
	Acf::istd
	Acf::ilog
	Acf::icomp
	Qt${QT_VERSION_MAJOR}::Test
	Qt${QT_VERSION_MAJOR}::Network
	Qt${QT_VERSION_MAJOR}::WebSockets
)

deploy_qt(TARGET ${PROJECT_NAME})
