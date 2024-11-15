TARGET = graphqlclient

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

# Set configuration of custom builds:
# ARX Compiler:
ARXC_CONFIG = $$PWD/../../../Config/ImtCore.awc
ARXC_FILES += $$PWD/../graphqlclient.acc
ARXC_OUTDIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET


include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)
include($(ACFDIR)/Config/QMake/CustomBuild.pri)
