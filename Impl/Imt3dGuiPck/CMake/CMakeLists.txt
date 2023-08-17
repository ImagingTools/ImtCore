project(Imt3dGuiPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)

if(QT_VERSION_MAJOR EQUAL 6)
	find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS OpenGLWidgets REQUIRED)
	target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
endif()

target_link_libraries(${PROJECT_NAME} imeas)
target_link_libraries(${PROJECT_NAME} imt3d imt3dgui imt3dview imtgui imtwidgets imtstyle)

