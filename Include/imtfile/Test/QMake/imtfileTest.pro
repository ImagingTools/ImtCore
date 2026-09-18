TARGET = imtfileTest

include($(ACFCONFIGDIR)/QMake/AppConfig.pri)
include($$PWD/../../../../Config/QMake/ImtCore.pri)

QT += testlib
CONFIG += console
CONFIG -= app_bundle

LIBS += -litest -limtfile
