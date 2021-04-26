TARGET = imtauthgui

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

QT += qml

FORMS += $$_PRO_FILE_PWD_/../*.ui
RESOURCES += $$_PRO_FILE_PWD_/../*.qrc

