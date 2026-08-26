project(TenantInvitationManagerTest)


include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtauth
	Acf::itest
	Qt${QT_VERSION_MAJOR}::Test
)

deploy_qt(TARGET ${PROJECT_NAME})

