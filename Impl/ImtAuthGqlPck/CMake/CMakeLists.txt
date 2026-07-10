project(ImtAuthGqlPck LANGUAGES CXX)


include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE}
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
	imtdoc
	imtmail
	imtbasesdl
	imtbase
	#
	Acf
	libs
	AcfSln::iauth
	Acf::idoc
	Acf::iqt
	Acf::i2d
	#
	Qt
	libs
	Qt${QT_VERSION_MAJOR}::Quick)

