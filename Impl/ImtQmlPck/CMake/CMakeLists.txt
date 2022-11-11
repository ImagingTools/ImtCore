project(ImtQmlPck LANGUAGES CXX)

find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS QuickWidgets REQUIRED)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::QuickWidgets)
target_link_libraries(${PROJECT_NAME} imtqml imtbase imtgql imtauth)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} User32 Advapi32)

	list(APPEND listOptions "--qmldir=${IMTCOREDIR}/Include/imtqml/Resources/qml")
	set(files  \"${IMTCOREDIR}/Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.arp\")

	windeploy(${PROJECT_NAME} "${listOptions}" "${files}")
	unset(listOptions)
endif()

