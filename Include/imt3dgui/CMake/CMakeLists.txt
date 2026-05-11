project(imt3dgui LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_IMAGES_INPUT_DIR "${PROJECT_SOURCE_DIR}/../Resources/Icons/Template")
set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR "${AUX_INCLUDE_DIR}/${PROJECT_NAME}")
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../Config/CMake/DesignTokenCreator.cmake)

# Compile RHI shaders (vshader_rhi.vert, fshader_rhi.frag) to QSB and embed
# them as Qt resources under :/RhiShaders/.
# Uses add_custom_command + Qt::rcc (same pattern as DdlCreator.cmake /
# ImtSdlConfig.cmake) instead of qt_add_shaders / qt_add_resources, which
# use keyword-style target_link_libraries and would conflict with the
# plain-style calls in the ACF / ImtCore build system.
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ShaderTools)

set(RHI_SHADER_SOURCES
    "${PROJECT_SOURCE_DIR}/../Resources/Shaders/vshader_rhi.vert"
    "${PROJECT_SOURCE_DIR}/../Resources/Shaders/fshader_rhi.frag"
)

set(RHI_SHADER_QSB_FILES)
foreach(SHADER_SRC IN LISTS RHI_SHADER_SOURCES)
    get_filename_component(SHADER_NAME "${SHADER_SRC}" NAME)
    set(QSB_OUT "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.qsb")
    add_custom_command(
        OUTPUT "${QSB_OUT}"
        COMMAND Qt${QT_VERSION_MAJOR}::qsb
        ARGS --glsl "100 es,120,150" --hlsl 50 --msl 12 -o "${QSB_OUT}" "${SHADER_SRC}"
        DEPENDS "${SHADER_SRC}"
        COMMENT "[imt3dgui] QSB ${SHADER_NAME}"
        VERBATIM
    )
    list(APPEND RHI_SHADER_QSB_FILES "${QSB_OUT}")
endforeach()

# Write the QRC at configure time; entries are generated from RHI_SHADER_SOURCES
# so that adding a new shader only requires updating the list above.
set(RHI_SHADERS_QRC "${CMAKE_CURRENT_BINARY_DIR}/rhi_shaders.qrc")
set(RHI_QRC_ENTRIES "")
foreach(SHADER_SRC IN LISTS RHI_SHADER_SOURCES)
    get_filename_component(SHADER_NAME "${SHADER_SRC}" NAME)
    string(APPEND RHI_QRC_ENTRIES
        "    <file alias=\"${SHADER_NAME}.qsb\">${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.qsb</file>\n")
endforeach()
file(WRITE "${RHI_SHADERS_QRC}"
    "<RCC>\n"
    "  <qresource prefix=\"/RhiShaders\">\n"
    "${RHI_QRC_ENTRIES}"
    "  </qresource>\n"
    "</RCC>\n"
)

# Run rcc to compile the QRC into a .cpp, after the .qsb files are ready.
set(RHI_SHADERS_CPP "${CMAKE_CURRENT_BINARY_DIR}/qrc_rhi_shaders.cpp")
add_custom_command(
    OUTPUT "${RHI_SHADERS_CPP}"
    COMMAND Qt${QT_VERSION_MAJOR}::rcc
    ARGS -name rhi_shaders "${RHI_SHADERS_QRC}" -o "${RHI_SHADERS_CPP}"
    DEPENDS "${RHI_SHADERS_QRC}" ${RHI_SHADER_QSB_FILES}
    COMMENT "[imt3dgui] rcc rhi_shaders.qrc"
    VERBATIM
)

target_sources(${PROJECT_NAME} PRIVATE "${RHI_SHADERS_CPP}")
