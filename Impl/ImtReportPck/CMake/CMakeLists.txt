project(ImtReportPck LANGUAGES CXX)


add_compile_definitions(COMMON_LIBRARY)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE}
	imtreport
	imtbase
	Qt${QT_VERSION_MAJOR}::PrintSupport
)

