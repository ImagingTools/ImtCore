project(EagleView LANGUAGES CXX)


set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/ImtCore.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../EagleView.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

if(WIN32)
	set(ACF_CONVERT_FILES "${PROJECT_SOURCE_DIR}/../VC/EagleView.rc.xtracf")
	set(ACF_CONVERT_REGISTRY "${PROJECT_SOURCE_DIR}/../VC/FileSubstitCopyApp.acc")
	set(ACF_CONVERT_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/BaseOnly.awc")
endif()

include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

if(QT_VERSION_MAJOR GREATER 4)
	set(EAGLE_VIEW_QT_LIBRARIES Qt${QT_VERSION_MAJOR}::PrintSupport)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtgui
	imtstyle
	imt3dgui
	Qt${QT_VERSION_MAJOR}::Xml
	${EAGLE_VIEW_QT_LIBRARIES}
)

deploy_qt(TARGET ${PROJECT_NAME})

