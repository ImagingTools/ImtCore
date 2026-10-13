project(ImtCoreQmlTest)


include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS QuickTest REQUIRED)

get_filename_component(IMT_QML_TEST_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

# Every tst_*.qml file below this directory is a test case
file(GLOB_RECURSE IMT_QML_TEST_FILES "${IMT_QML_TEST_DIR}/tst_*.qml")
add_custom_target(${PROJECT_NAME}-QML-files SOURCES ${IMT_QML_TEST_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE QUICK_TEST_SOURCE_DIR="${IMT_QML_TEST_DIR}")

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtbasesdl
	imtappsdl
	imtauthsdl
	imtcolorsdl
	imt2dsdl
	imtstylecontrolsqml
	imtcontrolsqml
	imtguiqml
	imtguigqlqml
	imtcolguiqml
	imtdocguiqml
	imtauthguiqml
	imtlicguiqml
	imtstyle
	imtqml
	imtgql
	Qt${QT_VERSION_MAJOR}::QuickTest
)

set(DEPLOY_QT_QML_DIRS "${IMT_QML_TEST_DIR}" "${IMTCOREDIR}/Qml")
deploy_qt(TARGET ${PROJECT_NAME} OPTIONS --include-plugins qoffscreen)

