TARGET = imtrest

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)
include($$PWD/../../../Config/QMake/Quazip.pri)

CONFIG += network
QT += websockets

RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)


