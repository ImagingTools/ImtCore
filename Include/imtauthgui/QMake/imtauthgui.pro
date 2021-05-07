TARGET = imtauthgui

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

QT += qml

FORMS += $$files($$_PRO_FILE_PWD_/../*.ui, true)
RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, true)

