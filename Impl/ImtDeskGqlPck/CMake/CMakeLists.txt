project(ImtDeskGqlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME}
	imtdesksdl
	imtdeskgql
	imtdesk
	imtservergql
	imtserverapp
	imtgql
	imtdb
	imtcol
	imtbasesdl
	imtbase
	iqt
	i2d)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
