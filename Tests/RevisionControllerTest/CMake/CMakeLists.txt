project(ImtCoreRevisionControllerTest)

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/WindeployQt.cmake)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(${PROJECT_NAME} itest ipackage imtauth imtbase)

imtcore_generate_tests(${TESTS_DIR}/RevisionControllerTest)
if(WIN32)
        set(listFiles \"${AUX_INCLUDE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}/${PROJECT_NAME}.exe\")

	windeploy(${PROJECT_NAME} "" "${listFiles}")
endif()
