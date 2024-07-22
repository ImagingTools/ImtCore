TARGET = imtrest

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)
include($(IACFDIR)/Config/QMake/zlib.pri)

CONFIG += network
QT += websockets

SOURCES += $$files($$_PRO_FILE_PWD_/../*.c, false)
RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)


