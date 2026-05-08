project(Imt3dDemo LANGUAGES CXX)

if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "off")
endif()

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

target_link_libraries(${PROJECT_NAME} imt3dgui imt3dview imt3d imtbase)

if(QT_VERSION_MAJOR EQUAL 6)
	target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
endif()

if(WIN32)
	set(files \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")
	windeploy(${PROJECT_NAME} "" "${files}")
endif()
