TARGET = imtdocgui

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

QT += qml quick

RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

