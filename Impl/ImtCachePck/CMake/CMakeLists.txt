project(ImtCachePck LANGUAGES CXX)


include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)
include(${IMTCOREDIR}/Config/CMake/DuckDb.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtcache
)

deploy_qt(TARGET ${PROJECT_NAME})
