project(imt3dTest)

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Test)

target_link_libraries(${PROJECT_NAME} ${ACF_APPLICATION_LINK_SCOPE} Acf::itest Acf::istd Acf::imath Acf::i3d Acf::iser imt3d Acf::iimg Acf::idoc AcfSln::icalib Acf::i2d)
target_link_libraries(${PROJECT_NAME} ${ACF_QT_MODULE_LINK_SCOPE} Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core)

target_link_libraries(${PROJECT_NAME} ${ACF_QT_MODULE_LINK_SCOPE} Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)

if(WIN32)
	set(listFiles \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")

	windeploy(${PROJECT_NAME} "" "${listFiles}")

endif()
