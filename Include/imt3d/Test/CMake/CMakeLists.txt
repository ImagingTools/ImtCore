project(imt3dTest)

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Test)

target_link_libraries(${PROJECT_NAME} itest istd imath i3d iser imt3d iimg idoc icalib i2d)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core)

acf_use_qt_graphics_modules()

if(WIN32)
	set(listFiles \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")

	windeploy(${PROJECT_NAME} "" "${listFiles}")

endif()
