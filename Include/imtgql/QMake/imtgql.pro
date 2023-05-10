TARGET = imtgql

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

CONFIG += network
QT += websockets
QT += qml quick quickwidgets


