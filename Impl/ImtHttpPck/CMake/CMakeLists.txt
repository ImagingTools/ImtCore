project(ImtHttpPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)

target_link_libraries(${PROJECT_NAME} 
	imtbase 
	imtcom 
	imthttp
	imtrepo
	imtbasesdl
	idoc
	iqt
	i2d
	istd
	Qt${QT_VERSION_MAJOR}::WebSockets)


