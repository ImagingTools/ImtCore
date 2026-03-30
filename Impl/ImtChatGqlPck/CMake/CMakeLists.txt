project(ImtChatGqlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME}
	imtchatsdl
	imtchatgql
	imtchat
	imtservergql
	imtgql
	imtdb
	imtcol
	imtbasesdl
	imtbase)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
