TARGET = imtbasetest

include($(ACFCONFIGDIR)/QMake/ApplicationConfig.pri)
include($$PWD/../../../../Config/QMake/ImtCore.pri)

LIBS += -L../../../../Lib/$$COMPILER_DIR
LIBS += -limtbase -litest

QT += core testlib widgets

CONFIG += console

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)
