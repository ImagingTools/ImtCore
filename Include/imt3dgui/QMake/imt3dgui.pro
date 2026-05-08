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
# We use QMAKE_EXTRA_COMPILERS so that the compiled .qsb files are generated
# before rcc processes imt3dgui.qrc (which references them).
QSB_TOOL = $$[QT_HOST_BINS]/qsb

QSB_SHADERS = \
    $$_PRO_FILE_PWD_/../Resources/Shaders/vshader_rhi.vert \
    $$_PRO_FILE_PWD_/../Resources/Shaders/fshader_rhi.frag

qsb_compiler.input = QSB_SHADERS
qsb_compiler.output = ${QMAKE_FILE_IN_PATH}/${QMAKE_FILE_IN_BASE}.${QMAKE_FILE_IN_EXT}.qsb
qsb_compiler.commands = $$QSB_TOOL --glsl \"100 es,120,150\" --hlsl 50 --msl 12 -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_IN}
qsb_compiler.name = QSB ${QMAKE_FILE_IN}
qsb_compiler.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += qsb_compiler

INCLUDEPATH += ../../

