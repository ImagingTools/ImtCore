project(imtdbTest)


# The engines that the tests check are the components of the composition Partitura/DatabaseEnginesTest.acc: the class of it
# (CDatabaseEnginesTest) is generated here by ARXC, the packages of the composition are searched in the directories of the configuration
set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../../Config/ImtCore.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../Partitura/DatabaseEnginesTest.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

# The dependencies of the composition and the generated files are written here, the directory of the target is another one
file(MAKE_DIRECTORY "${AUX_INCLUDE_DIR}/${PROJECT_NAME}")

include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

# The components of the packages that the composition uses
target_link_libraries(${PROJECT_NAME} PRIVATE
	imtdb
	Acf::ifile
	Acf::itest
	Qt${QT_VERSION_MAJOR}::Test
	Qt${QT_VERSION_MAJOR}::Sql
)

deploy_qt(TARGET ${PROJECT_NAME})
