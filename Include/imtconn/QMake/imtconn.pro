TARGET = imtconn

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

CONFIG += network
QT += websockets

SOURCES += $$files($$_PRO_FILE_PWD_/../*.cpp, false)


