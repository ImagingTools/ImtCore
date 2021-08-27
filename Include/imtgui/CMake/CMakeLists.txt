project(imtgui LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)


set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE "${IMTCOREDIR}/Include/imtgui/Resources/DesignToken.json")
set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_INPUT_DIR "${IMTCOREDIR}/Include/imtgui/Resources/Icons/Template")
set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR "${AUX_INCLUDE_DIR}/${PROJECT_NAME}")
set(QDESIGN_TOKEN_CREATOR_OUTPUT_FILE_NAMES imtguilight imtguidark)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../Config/CMake/DesigenTokenCreator.cmake)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)
