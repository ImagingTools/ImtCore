TARGET = ExternalLoginTest

include($(ACFCONFIGDIR)/QMake/ApplicationConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR
LIBS += -limtbase -limtauth -limtaccount -litest

QT += core network testlib widgets

CONFIG += console

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
