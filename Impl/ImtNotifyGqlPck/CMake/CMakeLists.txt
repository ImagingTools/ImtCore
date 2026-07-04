project(ImtNotifyGqlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME}
	imtnotifysdl
	imtnotifygql
	imtnotify
	imtservergql
	imtserverapp
	imtgql
	imtbasesdl
	imtbase
	iqt)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
