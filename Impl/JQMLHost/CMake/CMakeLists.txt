project(JQMLHost LANGUAGES CXX)


if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "off")
endif()

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

#------ SDL-generated QML libraries ------
target_link_libraries(${PROJECT_NAME} ${ACF_APPLICATION_LINK_SCOPE}
	imtappsdl
	imtauthsdl
	imtlicsdl
	imtcolorsdl
	imt2dsdl
	imtchatsdl
	imtdesksdl
	imtpaysdl)

#------ QML modules (from ImtCore/Qml) ------
target_link_libraries(${PROJECT_NAME} ${ACF_APPLICATION_LINK_SCOPE}
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
target_link_libraries(${PROJECT_NAME} ${ACF_APPLICATION_LINK_SCOPE}
	ImtCoreLoc
	Acf::AcfLoc
	AcfSln::AcfSlnLoc
	imtguigql
	imtauthdb
	imtauthgui
	imtlicgui
	imtstyle
	imtchatdb
	imtdeskdb
	imtlog)

#------ Qt modules ------
target_link_libraries(${PROJECT_NAME} ${ACF_APPLICATION_LINK_SCOPE}
	Qt${QT_VERSION_MAJOR}::QuickControls2
)

if(WIN32)
	list(APPEND listOptions "--qmldir=${IMTCOREDIR}/Qml")
	set(files \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")
	windeploy(${PROJECT_NAME} "${listOptions}" "${files}")
	unset(listOptions)
endif()

