project(imt3dgui LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_IMAGES_INPUT_DIR "${PROJECT_SOURCE_DIR}/../Resources/Icons/Template")
set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR "${AUX_INCLUDE_DIR}/${PROJECT_NAME}")
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../Config/CMake/DesignTokenCreator.cmake)

# Compile GLSL 450 shaders to QSB using the qsb tool and embed them as
# Qt resources under :/RhiShaders/.
# We invoke qsb and rcc directly (instead of qt_add_shaders) because
# qt_add_shaders internally calls qt_add_resources which uses keyword-style
# target_link_libraries, conflicting with the plain-style calls used by the
# ACF build system.
find_package(Qt6 COMPONENTS ShaderTools QUIET)
if(TARGET Qt6::ShaderTools)
    get_target_property(_qsb_exe Qt6::qsb IMPORTED_LOCATION)
    if(NOT _qsb_exe)
        set(_qsb_exe "${QT6_INSTALL_PREFIX}/bin/qsb")
    endif()

    set(_rhi_shader_srcdir "${CMAKE_CURRENT_SOURCE_DIR}/../Resources/Shaders")
    set(_rhi_shader_outdir "${CMAKE_CURRENT_BINARY_DIR}/RhiShaders")
    file(MAKE_DIRECTORY "${_rhi_shader_outdir}")

    set(_qsb_flags --batchable --precompile --optimize)

    set(_rhi_vert_src "${_rhi_shader_srcdir}/vshader_rhi.vert")
    set(_rhi_frag_src "${_rhi_shader_srcdir}/fshader_rhi.frag")
    set(_rhi_vert_qsb "${_rhi_shader_outdir}/vshader_rhi.vert.qsb")
    set(_rhi_frag_qsb "${_rhi_shader_outdir}/fshader_rhi.frag.qsb")

    add_custom_command(
        OUTPUT  "${_rhi_vert_qsb}"
        COMMAND "${_qsb_exe}" ${_qsb_flags} -o "${_rhi_vert_qsb}" "${_rhi_vert_src}"
        DEPENDS "${_rhi_vert_src}"
        COMMENT "Compiling vshader_rhi.vert -> QSB"
    )
    add_custom_command(
        OUTPUT  "${_rhi_frag_qsb}"
        COMMAND "${_qsb_exe}" ${_qsb_flags} -o "${_rhi_frag_qsb}" "${_rhi_frag_src}"
        DEPENDS "${_rhi_frag_src}"
        COMMENT "Compiling fshader_rhi.frag -> QSB"
    )

    # Generate a .qrc that embeds the compiled QSB files under :/RhiShaders/.
    set(_rhi_qrc "${_rhi_shader_outdir}/imt3dgui_rhi_shaders.qrc")
    file(WRITE "${_rhi_qrc}"
        "<RCC>\n<qresource prefix=\"/RhiShaders\">\n"
        "  <file alias=\"vshader_rhi.vert.qsb\">${_rhi_vert_qsb}</file>\n"
        "  <file alias=\"fshader_rhi.frag.qsb\">${_rhi_frag_qsb}</file>\n"
        "</qresource>\n</RCC>\n"
    )

    # Compile the .qrc to a .cpp via rcc (same pattern as DesignTokenCreator.cmake).
    set(_rhi_qrc_cpp "${_rhi_shader_outdir}/qrc_imt3dgui_rhi_shaders.cpp")
    add_custom_command(
        OUTPUT  "${_rhi_qrc_cpp}"
        COMMAND Qt${QT_VERSION_MAJOR}::rcc
        ARGS    -name imt3dgui_rhi_shaders "${_rhi_qrc}" -o "${_rhi_qrc_cpp}"
        DEPENDS "${_rhi_qrc}" "${_rhi_vert_qsb}" "${_rhi_frag_qsb}"
        COMMENT "Generating imt3dgui_rhi_shaders resource"
    )
    target_sources(${PROJECT_NAME} PRIVATE "${_rhi_qrc_cpp}")
else()
    message(WARNING "imt3dgui: Qt ShaderTools not found — CRhiRenderBackend shaders will not be compiled. "
                    "CRhiWidget / CRhiRenderBackend will not work at runtime.")
endif()
