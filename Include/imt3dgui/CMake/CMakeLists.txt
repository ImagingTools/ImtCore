project(imt3dgui LANGUAGES CXX)


include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

# Qt modules matching QMake's QT += gui-private opengl openglwidgets:
# - GuiPrivate  : exposes <rhi/qrhi.h> (private QRhi API used by CRhiRenderBackend).
#                 For Qt 6.8+ rhi/qrhi.h is a public header of Qt::Gui, so we link
#                 Qt::Gui directly.  For older versions fall back to GuiPrivate.
# - OpenGL / OpenGLWidgets : QOpenGLBuffer, QOpenGLShaderProgram, QOpenGLWidget
#                            (headers moved from QtGui to separate modules in Qt 6)

set(QT_VERSION_STR
    "${Qt${QT_VERSION_MAJOR}Core_VERSION_MAJOR}.${Qt${QT_VERSION_MAJOR}Core_VERSION_MINOR}.${Qt${QT_VERSION_MAJOR}Core_VERSION_PATCH}")

if(QT_VERSION VERSION_GREATER_EQUAL "6.8.0")
    # In Qt 6.8+, rhi/qrhi.h is a public header of Qt::Gui.
    # On macOS framework builds the versioned headers live inside
    # QtGui.framework and are not on the default include path.
    # Resolve the QtGui include root from the Gui target, handling both
    # framework layouts (.../QtGui.framework/Headers) and non-framework
    # layouts (.../include/QtGui).
    get_target_property(_qtgui_loc Qt${QT_VERSION_MAJOR}::Gui IMPORTED_LOCATION_RELEASE)
    if(NOT _qtgui_loc)
        get_target_property(_qtgui_loc Qt${QT_VERSION_MAJOR}::Gui IMPORTED_LOCATION)
    endif()

    set(QTGUI_HEADERS_DIR "")
    if(_qtgui_loc AND _qtgui_loc MATCHES "QtGui\\.framework")
        # Framework layout: <prefix>/lib/QtGui.framework/Versions/A/QtGui
        get_filename_component(_fw_dir "${_qtgui_loc}" DIRECTORY)  # .../Versions/A
        set(_fw_headers "${_fw_dir}/Headers")
        set(_fw_versioned "${_fw_headers}/${QT_VERSION_STR}/QtGui")
        if(EXISTS "${_fw_versioned}")
            set(QTGUI_HEADERS_DIR "${_fw_versioned}")
        elseif(EXISTS "${_fw_headers}")
            set(QTGUI_HEADERS_DIR "${_fw_headers}")
        endif()
    else()
        # Non-framework layout: try the standard include path
        get_target_property(_qtgui_inc Qt${QT_VERSION_MAJOR}::Gui INTERFACE_INCLUDE_DIRECTORIES)
        foreach(dir IN LISTS _qtgui_inc)
            file(TO_CMAKE_PATH "${dir}" dir_norm)
            if(dir_norm MATCHES "/QtGui$")
                set(_versioned "${dir_norm}/${QT_VERSION_STR}/QtGui")
                if(EXISTS "${_versioned}")
                    set(QTGUI_HEADERS_DIR "${_versioned}")
                else()
                    set(QTGUI_HEADERS_DIR "${dir_norm}")
                endif()
                break()
            endif()
        endforeach()
    endif()

    if(QTGUI_HEADERS_DIR)
        target_include_directories(${PROJECT_NAME} PUBLIC "${QTGUI_HEADERS_DIR}")
    endif()
else()
    # Pre-6.8: rhi/qrhi.h is a private header, need the private module's
    # include directories (but NOT linking GuiPrivate — headers only via
    # the versioned include path resolved above would be the 6.8+ path;
    # for older Qt the private header is only reachable through GuiPrivate).
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS GuiPrivate QUIET)
    if(TARGET Qt${QT_VERSION_MAJOR}::GuiPrivate)
        get_target_property(_gp_incs Qt${QT_VERSION_MAJOR}::GuiPrivate INTERFACE_INCLUDE_DIRECTORIES)
        foreach(dir IN LISTS _gp_incs)
            target_include_directories(${PROJECT_NAME} PUBLIC "${dir}")
        endforeach()
    endif()
endif()

set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_IMAGES_INPUT_DIR "${PROJECT_SOURCE_DIR}/../Resources/Icons/Template")
set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR "${AUX_INCLUDE_DIR}/${PROJECT_NAME}")
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../Config/CMake/DesignTokenCreator.cmake)

# --- RHI shader compilation (mirrors QMake qsb_compiler in imt3dgui.pro) ---
# Pre-compiled .qsb files are committed to the repository so that builds
# without Qt ShaderTools still work.  When ShaderTools is available the
# shaders are recompiled from source, keeping the checked-in copies up to date.

set(RHI_SHADER_DIR "${PROJECT_SOURCE_DIR}/../Resources/Shaders")

set(RHI_SHADER_SOURCES
    ${RHI_SHADER_DIR}/vshader_rhi.vert
    ${RHI_SHADER_DIR}/fshader_rhi.frag
)

foreach(SHADER_SRC ${RHI_SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER_SRC} NAME)
    list(APPEND QSB_OUTPUTS "${RHI_SHADER_DIR}/${SHADER_NAME}.qsb")
endforeach()

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ShaderTools QUIET)

if(TARGET Qt${QT_VERSION_MAJOR}::qsb)
    foreach(SHADER_SRC ${RHI_SHADER_SOURCES})
        get_filename_component(SHADER_NAME ${SHADER_SRC} NAME)
        set(QSB_OUTPUT "${RHI_SHADER_DIR}/${SHADER_NAME}.qsb")

        add_custom_command(
            OUTPUT ${QSB_OUTPUT}
            COMMAND Qt${QT_VERSION_MAJOR}::qsb
                --glsl "100 es,120,150" --hlsl 50 --msl 12
                -o ${QSB_OUTPUT} ${SHADER_SRC}
            DEPENDS ${SHADER_SRC}
            COMMENT "[imt3dgui] Compiling RHI shader ${SHADER_NAME} -> ${SHADER_NAME}.qsb"
            VERBATIM
        )
    endforeach()
endif()

# --- Compile the static .qrc (which references the .qsb files) via rcc ---
set(IMT3DGUI_QRC_FILE "${PROJECT_SOURCE_DIR}/../imt3dgui.qrc")
set(IMT3DGUI_QRC_CPP  "${CMAKE_CURRENT_BINARY_DIR}/qrc_imt3dgui.cpp")

add_custom_command(
    OUTPUT ${IMT3DGUI_QRC_CPP}
    COMMAND Qt${QT_VERSION_MAJOR}::rcc
        -name imt3dgui ${IMT3DGUI_QRC_FILE} -o ${IMT3DGUI_QRC_CPP}
    DEPENDS ${IMT3DGUI_QRC_FILE} ${QSB_OUTPUTS}
    COMMENT "[imt3dgui] CREATING RES ${IMT3DGUI_QRC_CPP}"
    VERBATIM
)

target_sources(${PROJECT_NAME} PRIVATE ${IMT3DGUI_QRC_CPP})
