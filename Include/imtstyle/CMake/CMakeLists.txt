project(imtstyle LANGUAGES CXX)


include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_QT_MODULE_LINK_SCOPE} Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)

set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_IMAGES_INPUT_DIR "${PROJECT_SOURCE_DIR}/../Resources/Icons/Template")
set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_STYLES_INPUT_DIR "${PROJECT_SOURCE_DIR}/../Resources/Styles/Template")
set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR "${AUX_INCLUDE_DIR}/${PROJECT_NAME}")
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../Config/CMake/DesignTokenCreator.cmake)



