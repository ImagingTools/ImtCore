project(DesignTokenCreator LANGUAGES CXX)


set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/BaseOnly.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

message(VERBOSE "ARXC_CONFIG " ${ARXC_CONFIG})

if(WIN32)
	set(ACF_CONVERT_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/BaseOnly.awc")
endif()

include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtdesign
)

deploy_qt(TARGET ${PROJECT_NAME})

