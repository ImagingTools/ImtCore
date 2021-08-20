TARGET = imtgui

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

#DESIGN TOKEN CREATOR config
DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE = $$(IMTCOREDIR)/Include/imtgui/Resources/DesignToken.json
DESIGN_TOKEN_CREATOR_COMMAND_PARAM_INPUT_DIR = $$(IMTCOREDIR)/Include/imtgui/Resources/Icons/Template
DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR = $$(IMTCOREDIR)/Include/imtgui/Resources/Icons/Output
include($$PWD/../../../Config/QMake/DesigenTokenCreator.pri)

FORMS += $$files($$_PRO_FILE_PWD_/../*.ui, false)
RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

