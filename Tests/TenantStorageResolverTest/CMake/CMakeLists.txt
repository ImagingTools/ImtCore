project(TenantStorageResolverTest)


include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtdb
	Acf::itest
	Qt${QT_VERSION_MAJOR}::Test
	Qt${QT_VERSION_MAJOR}::Concurrent
)

deploy_qt(TARGET ${PROJECT_NAME})
