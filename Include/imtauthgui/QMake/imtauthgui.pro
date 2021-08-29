TARGET = imtauthgui

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

DESIGN_TOKEN_CREATOR_COMMAND_PARAM_INPUT_DIR = $$_PRO_FILE_/../../Resources/Icons/Template
DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET
include($$PWD/../../../Config/QMake/DesigenTokenCreator.pri)

QT += qml quick

FORMS += $$files($$_PRO_FILE_PWD_/../*.ui, false)
RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

