project(ImtRestPck LANGUAGES CXX)

message(VERBOSE "QT_VERSION_MAJOR ${QT_VERSION_MAJOR}")

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)


target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE}
	imtrepo
	imtrest
	imtcom
	imtbase
	imtbasesdl
	Acf::idoc
	Acf::iqt
	Acf::i2d
	Acf::istd
	Qt${QT_VERSION_MAJOR}::WebSockets)

if (WIN32)
	set(files \"${IMTCOREDIR_BUILD}/Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.arp\")
	
	windeploy(${PROJECT_NAME} "" "${files}")
endif ()
