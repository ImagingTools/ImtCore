project(ImtAuthGqlPck LANGUAGES CXX)


include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME}
	imtauthsdl
	imtauthgql
	imtqml
	imtlic
	imtguigql
	imtservergql
	imtserverapp
	imtclientgql
	imtgql
	imtdb
	imtcol
	imtmail
	imtbasesdl
	imtbase
	# Acf libs
	iauth
	idoc
	iqt
	i2d
	# Qt libs
	Qt${QT_VERSION_MAJOR}::Quick)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
