project(TenantIsolationPostgresTest)


include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtbase
	imtdb
	Acf::itest
	Qt${QT_VERSION_MAJOR}::Test
	Qt${QT_VERSION_MAJOR}::Concurrent
	Qt${QT_VERSION_MAJOR}::Sql
)

deploy_qt(TARGET ${PROJECT_NAME})
