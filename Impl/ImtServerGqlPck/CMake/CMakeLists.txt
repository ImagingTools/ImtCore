project(ImtServerGqlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
target_link_libraries(${PROJECT_NAME}
	imtrepo
	imtcom
	imtdoc
	imtrest
	imtfile
	imtservergql
	imtserverapp
	imthype
	imtgql
	imtcol
	imtbasesdl
	imtbase)

include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)

if(NOT DEFINED USE_QTQML OR USE_QTQML)
	target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::QuickWidgets)
endif()



