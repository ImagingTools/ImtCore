TARGET = imtauthgui

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

DESIGN_TOKEN_CREATOR_COMMAND_PARAM_IMAGES_INPUT_DIR = $$_PRO_FILE_/../../Resources/Icons/Template
DESIGN_TOKEN_CREATOR_COMMAND_PARAM_STYIES_INPUT_DIR = $$_PRO_FILE_/../../Resources/Styles/Template
DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET
DESIGN_TOKEN_CREATOR_INPUT_THEME_BASE_PATH = $$(IMTCOREDIR)/Include/imtstyle
include($$PWD/../../../Config/QMake/DesignTokenCreator.pri)

QT += qml quick

FORMS += $$files($$_PRO_FILE_PWD_/../*.ui, false)
RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

