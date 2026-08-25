project(ImtRestPck LANGUAGES CXX)


message(VERBOSE "QT_VERSION_MAJOR ${QT_VERSION_MAJOR}")

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtrest
)

deploy_qt(TARGET ${PROJECT_NAME})

