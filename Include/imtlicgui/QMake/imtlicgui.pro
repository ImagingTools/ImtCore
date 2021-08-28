TARGET = imtlicgui

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

FORMS += $$files($$_PRO_FILE_PWD_/../*.ui, false)
RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

DESIGN_TOKEN_CREATOR_COMMAND_PARAM_INPUT_DIR = $$_PRO_FILE_/../../Resources/Icons/Template
DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET
DESIGN_TOKEN_CREATOR_INPUT_THEME_FILES = $$files($$_PRO_FILE_/../../*.theme, false)
include($$PWD/../../../Config/QMake/DesigenTokenCreator.pri)
