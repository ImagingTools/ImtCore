project(imt2d LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_LIBRARY_LINK_SCOPE}
	imt2dsdl
	imtcolorsdl
	Acf::istd
)

target_link_libraries(${PROJECT_NAME} ${ACF_QT_MODULE_LINK_SCOPE}
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Gui
)
