project(ImtDuckDbPck LANGUAGES CXX)


include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

include(${IMTCOREDIR}/Config/CMake/DuckDb.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtduckdb
)

deploy_qt(TARGET ${PROJECT_NAME})
