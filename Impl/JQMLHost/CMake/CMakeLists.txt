project(JQMLHost LANGUAGES CXX)


if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "off")
endif()

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS QuickTest REQUIRED)

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
	Qt${QT_VERSION_MAJOR}::QuickTest
)

set(DEPLOY_QT_QML_DIRS "${IMTCOREDIR}/Qml")
deploy_qt(TARGET ${PROJECT_NAME})

get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
if(_qmake_executable)
	execute_process(
		COMMAND "${_qmake_executable}" -query QT_INSTALL_QML
		OUTPUT_VARIABLE QT_INSTALL_QML
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	execute_process(
		COMMAND "${_qmake_executable}" -query QT_INSTALL_BINS
		OUTPUT_VARIABLE QT_INSTALL_BINS
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	if(EXISTS "${QT_INSTALL_QML}/QtTest")
		set(_qttest_copy_cmds
			COMMAND ${CMAKE_COMMAND} -E copy_directory
				"${QT_INSTALL_QML}/QtTest"
				"$<TARGET_FILE_DIR:${PROJECT_NAME}>/qml/QtTest"
		)
		foreach(_qttest_dll IN ITEMS
			Qt6Test.dll
			Qt6Testd.dll
			Qt6QuickTest.dll
			Qt6QuickTestd.dll
		)
			if(EXISTS "${QT_INSTALL_BINS}/${_qttest_dll}")
				list(APPEND _qttest_copy_cmds
					COMMAND ${CMAKE_COMMAND} -E copy_if_different
						"${QT_INSTALL_BINS}/${_qttest_dll}"
						"$<TARGET_FILE_DIR:${PROJECT_NAME}>"
				)
			endif()
		endforeach()
		add_custom_command(
			TARGET ${PROJECT_NAME}
			POST_BUILD
			${_qttest_copy_cmds}
			COMMENT "Deploying QtTest QML module and libraries for JQMLHost"
		)
	endif()
endif()

