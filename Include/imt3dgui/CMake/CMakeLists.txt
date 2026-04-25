project(imt3dgui LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_IMAGES_INPUT_DIR "${PROJECT_SOURCE_DIR}/../Resources/Icons/Template")
set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR "${AUX_INCLUDE_DIR}/${PROJECT_NAME}")
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../Config/CMake/DesignTokenCreator.cmake)

# Compile GLSL 450 shaders to QSB and embed them as Qt resources under :/RhiShaders/
# Requires Qt 6.6+ (find_package(Qt6 COMPONENTS ShaderTools)).
find_package(Qt6 COMPONENTS ShaderTools QUIET)
if(TARGET Qt6::ShaderTools)
    qt_add_shaders(${PROJECT_NAME} "imt3dgui_rhi_shaders"
        BATCHABLE
        PRECOMPILE
        OPTIMIZED
        PREFIX "/RhiShaders"
        FILES
            ../Resources/Shaders/vshader_rhi.vert
            ../Resources/Shaders/fshader_rhi.frag
    )
else()
    message(WARNING "imt3dgui: Qt ShaderTools not found — CRhiRenderBackend shaders will not be compiled. "
                    "CRhiWidget / CRhiRenderBackend will not work at runtime.")
endif()
