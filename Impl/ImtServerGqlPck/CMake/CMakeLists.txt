project(ImtServerGqlPck LANGUAGES CXX)


include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE}
	imtrepo
	imtservergql
)

if(NOT DEFINED USE_QTQML OR USE_QTQML)
	target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE}
		Qt${QT_VERSION_MAJOR}::QuickWidgets)
endif()

