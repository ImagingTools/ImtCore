project(ImtGeoPck LANGUAGES CXX)


message(VERBOSE "QT_VERSION_MAJOR ${QT_VERSION_MAJOR}")

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Sql REQUIRED)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtgeo
)

deploy_qt(TARGET ${PROJECT_NAME})

