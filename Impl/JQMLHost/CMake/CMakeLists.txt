project(JQMLHost LANGUAGES CXX)


if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "off")
endif()

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtappsdl
	imtauthsdl
	imtlicsdl
	imtcolorsdl
	imt2dsdl
	imtchatsdl
	imtdesksdl
	imtpaysdl
	imtcontrolsqml
	imtstylecontrolsqml
	imtguiqml
	imtguigqlqml
	imtcolguiqml
	imtdocguiqml
	imtauthguiqml
	imtlicguiqml
	imtchatguiqml
	imtdeskguiqml
	ImtCoreLoc
	imtguigql
	imtauthdb
	imtauthgui
	imtlicgui
	imtstyle
	imtchatdb
	imtdeskdb
	imtlog
	Qt${QT_VERSION_MAJOR}::QuickControls2
)

set(DEPLOY_QT_QML_DIRS "${IMTCOREDIR}/Qml")
deploy_qt(TARGET ${PROJECT_NAME})

