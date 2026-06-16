project(JQMLHost LANGUAGES CXX)

if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "off")
endif()

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

#------ SDL-generated QML libraries ------
target_link_libraries(${PROJECT_NAME}
	imtbasesdl
	imtappsdl
	imtauthsdl
	imtlicsdl
	imtcolorsdl
	imt2dsdl
	imtchatsdl
	imtdesksdl
	imtpaysdl)

#------ QML modules (from ImtCore/Qml) ------
target_link_libraries(${PROJECT_NAME}
	imtcontrolsqml
	imtstylecontrolsqml
	imtguiqml
	imtguigqlqml
	imtcolguiqml
	imtdocguiqml
	imtauthguiqml
	imtlicguiqml
	imtchatguiqml
	imtdeskguiqml)

#------ Core ImtCore libraries ------
target_link_libraries(${PROJECT_NAME}
	imtbase
	ImtCoreLoc
	AcfLoc
	AcfSlnLoc
	imtdb
	imtgui
	imtguigql
	imtauth
	imtauthgui
	imtlic
	imtlicgui
	imtstyle
	imtqml
	imtcol
	imtgql
	imtclientgql
	imtserverapp
	imtservice
	imtcom
	imtlog
	imtcrypt
	imtrest
	imtapp
	imtchat
	imtdesk)

#------ Qt modules ------
target_link_libraries(${PROJECT_NAME}
	Qt${QT_VERSION_MAJOR}::Qml
	Qt${QT_VERSION_MAJOR}::Quick
	Qt${QT_VERSION_MAJOR}::QuickControls2
	Qt${QT_VERSION_MAJOR}::QuickWidgets
	Qt${QT_VERSION_MAJOR}::WebSockets)

if(WIN32)
	list(APPEND listOptions "--qmldir=${IMTCOREDIR}/Qml")
	set(files \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")
	windeploy(${PROJECT_NAME} "${listOptions}" "${files}")
	unset(listOptions)
endif()
