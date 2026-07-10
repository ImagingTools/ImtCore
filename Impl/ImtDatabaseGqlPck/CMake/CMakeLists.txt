project(ImtDatabaseGqlPck LANGUAGES CXX)

message(VERBOSE "QT_VERSION_MAJOR ${QT_VERSION_MAJOR}")


include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} imtgql imtdbgql imtrepo imtcom imtrest imtfile imtbase imtdb imtservergql)

include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

if(NOT DEFINED USE_QTQML OR USE_QTQML)
	target_link_libraries(${PROJECT_NAME} ${ACF_QT_MODULE_LINK_SCOPE} Qt${QT_VERSION_MAJOR}::QuickWidgets)
endif()



