project(ImtGeoPck LANGUAGES CXX)

message(VERBOSE "QT_VERSION_MAJOR ${QT_VERSION_MAJOR}")

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Sql REQUIRED)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

target_link_libraries(${PROJECT_NAME}
	imtserverapp
	imtgeo
	imtguigql
	imtservergql
	imtcol
	imtgql
	imtbasesdl
	imtdb
	imtbase
	Qt${QT_VERSION_MAJOR}::Sql
)

include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)

if(WIN32)
	set(files \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.arp\")
	windeploy(${PROJECT_NAME} "" "${files}")
endif()
