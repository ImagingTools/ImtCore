TARGET = imtsdlgencpptest

include($(ACFCONFIGDIR)/QMake/ApplicationConfig.pri)
include($$PWD/../../../../Config/QMake/ImtCore.pri)

LIBS += -L../../../../Lib/$$COMPILER_DIR
LIBS += -lImtSdlGenCppPck -limtsdlgencpp -limtsdl -limtbase -litest

QT += core testlib widgets

CONFIG += console

# Set configuration of custom builds:
# ARX Compiler:
ARXC_CONFIG = $$PWD/../../../../Config/ImtCore.awc
ARXC_FILES += $$PWD/../ImtSdlGenTest.acc
ARXC_OUTDIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)
include($(ACFDIR)/Config/QMake/CustomBuild.pri)
