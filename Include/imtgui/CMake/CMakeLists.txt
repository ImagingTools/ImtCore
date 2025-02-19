project(imtgui LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_IMAGES_INPUT_DIR "${PROJECT_SOURCE_DIR}/../Resources/Icons/Template")
set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_STYIES_INPUT_DIR "${PROJECT_SOURCE_DIR}/../Resources/Styles/Template")
set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR "${AUX_INCLUDE_DIR}/${PROJECT_NAME}")
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../Config/CMake/DesignTokenCreator.cmake)

set(MODULE_CPP_NAME "imtgui")
set(MODULE_QML_NAME "imtguivars")
set(DDL_CONST_VAR_TEMPLATE_INPUT_DIR "${PROJECT_SOURCE_DIR}/../Resources/Ddl")
set(DDL_OUTPUT_DIR "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/Ddl")

# set(DDL_CPP_OUTPUT_DIR "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/Ddl/Cpp")
# set(DDL_QML_OUTPUT_DIR "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/Ddl/Qml/imtguivars")

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../Config/CMake/DdlCreator.cmake)

target_link_libraries(${PROJECT_NAME} imtcol)

GenerateDdlCode(${MODULE_CPP_NAME}
	${MODULE_QML_NAME}
	${DDL_CONST_VAR_TEMPLATE_INPUT_DIR}
	${DDL_OUTPUT_DIR})

# GenerateDdlCodeExt(${MODULE_CPP_NAME}
# 	${MODULE_QML_NAME}
# 	${DDL_CONST_VAR_TEMPLATE_INPUT_DIR}
# 	${DDL_CPP_OUTPUT_DIR}
# 	${DDL_QML_OUTPUT_DIR})

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)
