project(imt3dgui LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_INPUT_DIR "${PROJECT_SOURCE_DIR}/../Resources/Icons/Template")
set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR "${AUX_INCLUDE_DIR}/${PROJECT_NAME}")
file(GLOB DESIGN_TOKEN_CREATOR_INPUT_THEME_FILES "${PROJECT_SOURCE_DIR}/../*.theme")
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../Config/CMake/DesigenTokenCreator.cmake)
