project(ImtCoreQmlTest)


include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlCustomConfig.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS QuickTest REQUIRED)

get_filename_component(IMT_QML_TEST_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

# Every tst_*.qml file below this directory is a test case
file(GLOB_RECURSE IMT_QML_TEST_FILES "${IMT_QML_TEST_DIR}/tst_*.qml")
add_custom_target(${PROJECT_NAME}-QML-files SOURCES ${IMT_QML_TEST_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE QUICK_TEST_SOURCE_DIR="${IMT_QML_TEST_DIR}")

# The C++ and QML code of the test schema 'ScalarUnion.sdl' (the same schema is used by ImtSdlGenIntegrationTest).
# The library name must be '<namespace>sdl', because the QML module of the schema is '<namespace><Schema>Sdl'
# The library is already defined, if ImtSdlGenIntegrationTest is configured
if (NOT TARGET sdlgentestunionsdl)
	add_library(sdlgentestunionsdl STATIC)

	block()
		set(PROJECT_NAME sdlgentestunionsdl)
		ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH "${IMTCOREDIR}/Include/imtsdlgencpp/Test/TestData/ScalarUnion.sdl"
			VERSION "1.0")
	endblock()

	add_dependencies(sdlgentestunionsdl SdlCodeGenerator)

	target_link_libraries(sdlgentestunionsdl PUBLIC
		imtbase
		imtgql
		imtservergql
	)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
	sdlgentestunionsdl
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
if (APPLE)
	deploy_qt(TARGET ${PROJECT_NAME})
else()
	deploy_qt(TARGET ${PROJECT_NAME} OPTIONS --include-plugins qoffscreen)
endif()

