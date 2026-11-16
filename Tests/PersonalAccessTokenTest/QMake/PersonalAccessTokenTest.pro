TARGET = PersonalAccessTokenTest

include($(ACFCONFIGDIR)/QMake/ApplicationConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR
LIBS += -limtbase -limtauth -limtaccount -limtauthdb -limtdb -litest

QT += core testlib widgets sql

CONFIG += console

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
