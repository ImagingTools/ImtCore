TARGET = imt3dgui

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

# Qt RHI widget (QRhiWidget) requires Qt >= 6.7; shader-tools for pre-compiled QSB.
# opengl / openglwidgets are needed for QOpenGLBuffer, QOpenGLShaderProgram,
# and QOpenGLWidget; the headers moved from QtGui to separate modules in Qt 6.
greaterThan(QT_MAJOR_VERSION, 5) {
    QT += opengl openglwidgets gui-private
}
QT += shadertools

DESIGN_TOKEN_CREATOR_COMMAND_PARAM_IMAGES_INPUT_DIR = $$_PRO_FILE_/../../Resources/Icons/Template
DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET
DESIGN_TOKEN_CREATOR_INPUT_THEME_BASE_PATH = $$(IMTCOREDIR)/Include/imtstyle
include($$PWD/../../../Config/QMake/DesignTokenCreator.pri)

#FORMS += $$files($$_PRO_FILE_PWD_/../*.ui, false)
RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

# Compile GLSL 450 shaders to QSB and embed them as Qt resources under :/RhiShaders/
# We use explicit QMAKE_EXTRA_TARGETS instead of QMAKE_SHADER_FILES so that the
# compiled .qsb files end up in imt3dgui.qrc with the /RhiShaders prefix that
# the C++ code (CRhiRenderBackend, CRender3dItem) expects.
QSB_TOOL = $$[QT_HOST_BINS]/qsb

QSB_VERT_IN  = $$_PRO_FILE_PWD_/../Resources/Shaders/vshader_rhi.vert
QSB_FRAG_IN  = $$_PRO_FILE_PWD_/../Resources/Shaders/fshader_rhi.frag
QSB_VERT_OUT = $$_PRO_FILE_PWD_/../Resources/Shaders/vshader_rhi.vert.qsb
QSB_FRAG_OUT = $$_PRO_FILE_PWD_/../Resources/Shaders/fshader_rhi.frag.qsb

compile_vert_shader.target = $$QSB_VERT_OUT
compile_vert_shader.depends = $$QSB_VERT_IN
compile_vert_shader.commands = $$QSB_TOOL --glsl \"100 es,120,150\" --hlsl 50 --msl 12 -o $$QSB_VERT_OUT $$QSB_VERT_IN
QMAKE_EXTRA_TARGETS += compile_vert_shader

compile_frag_shader.target = $$QSB_FRAG_OUT
compile_frag_shader.depends = $$QSB_FRAG_IN
compile_frag_shader.commands = $$QSB_TOOL --glsl \"100 es,120,150\" --hlsl 50 --msl 12 -o $$QSB_FRAG_OUT $$QSB_FRAG_IN
QMAKE_EXTRA_TARGETS += compile_frag_shader

PRE_TARGETDEPS += $$QSB_VERT_OUT $$QSB_FRAG_OUT

INCLUDEPATH += ../../

