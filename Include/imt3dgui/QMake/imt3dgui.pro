TARGET = imt3dgui

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

# Qt RHI widget (QRhiWidget) requires Qt >= 6.7; shader-tools for pre-compiled QSB.
# opengl / openglwidgets are needed for QOpenGLBuffer, QOpenGLShaderProgram,
# and QOpenGLWidget; the headers moved from QtGui to separate modules in Qt 6.
greaterThan(QT_MAJOR_VERSION, 5) {
    QT += opengl openglwidgets
}
QT += shadertools

DESIGN_TOKEN_CREATOR_COMMAND_PARAM_IMAGES_INPUT_DIR = $$_PRO_FILE_/../../Resources/Icons/Template
DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET
DESIGN_TOKEN_CREATOR_INPUT_THEME_BASE_PATH = $$(IMTCOREDIR)/Include/imtstyle
include($$PWD/../../../Config/QMake/DesignTokenCreator.pri)

#FORMS += $$files($$_PRO_FILE_PWD_/../*.ui, false)
RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

# Compile GLSL 450 shaders to QSB and embed them as Qt resources under :/RhiShaders/
QMAKE_SHADER_FILES += \
    $$_PRO_FILE_PWD_/../Resources/Shaders/vshader_rhi.vert \
    $$_PRO_FILE_PWD_/../Resources/Shaders/fshader_rhi.frag

INCLUDEPATH += ../../

