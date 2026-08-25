project(ImtSdlGenQmlTest)


set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../../Config/BaseOnly.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")
set (ARX_ENABLE_GENERATE_DEPENDENCIES_LIST OFF)

if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "off")
endif()

include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtsdlgenqml
	imtsdlgencpp
	imtsdl
	imtfile
	imtbase
	Acf::itest
	Qt${QT_VERSION_MAJOR}::Test
)

deploy_qt(TARGET ${PROJECT_NAME})

