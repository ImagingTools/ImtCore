project(Imt3dGuiPck LANGUAGES CXX)


include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

if(NOT TARGET Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
	find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS OpenGLWidgets REQUIRED)
endif()

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE}
	imt3dgui
	Qt${QT_VERSION_MAJOR}::OpenGLWidgets
)

