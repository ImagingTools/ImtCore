project(GraphQlServerTest LANGUAGES CXX)


# if(APPLE)
# 	set(CMAKE_MACOSX_BUNDLE "on")
# endif()

# add_executable(${PROJECT_NAME} ../Main.cpp)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)
include(${IMTCOREDIR}/Config/CMake/Quazip.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

target_link_libraries(${PROJECT_NAME} graphqlserver)
# target_link_libraries(${PROJECT_NAME} imtbase imtgui imtauth imtauthgui imtlicdb imtlic imtlicgui imtrest imtrepo imtstyle imtqml imtdb imtfile imtcom imtlicsdl imtappsdl imtauthsdl imtbasesdl imtservergql imtcol)
# target_link_libraries(${PROJECT_NAME} graphqlserver imtlicgql imtguigql imtclientgql imtgql imtauthgql imtauthdb imtapp imtservice imtlog)
# target_link_libraries(${PROJECT_NAME} ImtCoreLoc)
# target_link_libraries(${PROJECT_NAME} imtstylecontrolsqml)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::QuickWidgets  Qt${QT_VERSION_MAJOR}::WebSockets)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(${PROJECT_NAME} imtgui)


if(DEFINED WEB_COMPILE AND  WEB_COMPILE)
	add_compile_definitions(WEB_COMPILE)
endif()


