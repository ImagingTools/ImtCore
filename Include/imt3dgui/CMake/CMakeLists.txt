project(imt3dgui LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_IMAGES_INPUT_DIR "${PROJECT_SOURCE_DIR}/../Resources/Icons/Template")
set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR "${AUX_INCLUDE_DIR}/${PROJECT_NAME}")
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../Config/CMake/DesignTokenCreator.cmake)

# RHI shaders (vshader_rhi.vert, fshader_rhi.frag) are compiled to QSB via
# QMAKE_SHADER_FILES in the QMake build path (see QMake/imt3dgui.pro).
# CMake shader compilation is intentionally omitted because Qt's
# qt_add_shaders / qt_add_resources macros use keyword-style
# target_link_libraries, which conflicts with the plain-style calls in the
# ACF / ImtCore build system.  CRhiRenderBackend::Initialize() handles the
# missing-shaders case gracefully (returns false, OpenGL backend is used).
