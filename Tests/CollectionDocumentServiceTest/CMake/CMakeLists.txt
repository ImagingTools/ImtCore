project(CollectionDocumentServiceTest)


include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtdoc
	Acf::itest
	Qt${QT_VERSION_MAJOR}::Test
)

deploy_qt(TARGET ${PROJECT_NAME})

