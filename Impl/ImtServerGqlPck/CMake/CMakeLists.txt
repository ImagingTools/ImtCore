project(ImtServerGqlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE}
	imtrepo
	imtcom
	imtauth
	imtrest
	imtfile
	imtservergql
	imtserverapp
	imtdoc
	imthype
	imtgql
	imtcol
	imtbasesdl
	imtbase)

include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

if(NOT DEFINED USE_QTQML OR USE_QTQML)
	target_link_libraries(${PROJECT_NAME} ${ACF_QT_MODULE_LINK_SCOPE} Qt${QT_VERSION_MAJOR}::QuickWidgets)
endif()



