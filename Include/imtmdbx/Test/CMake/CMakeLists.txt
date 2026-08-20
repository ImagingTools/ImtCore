project(imtmdbxTest)


include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/Mdbx.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)


target_link_libraries(${PROJECT_NAME} PRIVATE
	imtmdbx
	Acf::itest
	Qt${QT_VERSION_MAJOR}::Test
)

if(WIN32)
	set(listFiles \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")
	windeploy(${PROJECT_NAME} "" "${listFiles}")
endif()

