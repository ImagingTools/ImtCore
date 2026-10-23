project(imtrestdemoTest)


# The tests check the server of the demo application: the class of it (CImtRestDemoServer) is generated here from the same partitura
set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../../Config/ImtCore.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../../../../Impl/ImtRestDemoServer/ImtRestDemoServer.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

# The dependencies of the composition and the generated files are written here, the directory of the target is another one
file(MAKE_DIRECTORY "${AUX_INCLUDE_DIR}/${PROJECT_NAME}")

include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtrestdemo
	Acf::itest
	Qt${QT_VERSION_MAJOR}::Test
)

deploy_qt(TARGET ${PROJECT_NAME})
