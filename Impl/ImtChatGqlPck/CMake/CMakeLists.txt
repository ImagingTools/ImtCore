project(ImtChatGqlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE}
	imtchatsdl
	imtchatgql
	imtchat
	imtservergql
	imtserverapp
	imtgql
	imtdb
	imtcol
	imtbasesdl
	imtbase
	Acf::iqt
	Acf::i2d)

