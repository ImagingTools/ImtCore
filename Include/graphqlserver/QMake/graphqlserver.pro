TARGET = graphqlserver

# include($(ACFDIR)/Config/QMake/StaticConfig.pri)
# include($$PWD/../../../Config/QMake/ImtCore.pri)
include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

INCLUDEPATH += $(ACFDIR)/Impl
INCLUDEPATH += $(IMTCOREDIR)/Impl

# Set configuration of custom builds:
# ARX Compiler:
ARXC_CONFIG = $$PWD/../../../Config/ImtCore.awc
ARXC_FILES += $$PWD/../graphqlserver.acc
ARXC_OUTDIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET


include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)
include($(ACFDIR)/Config/QMake/CustomBuild.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR
LIBS += -limtbase -limtgql -limtrest -limtservergql -limtauth -limtauthgql -limtservice -limtclientgql -limtgui

