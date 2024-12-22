include($(ACFDIR)/Config/QMake/SharedLibraryConfig.pri)
include($(ACFDIR)/Config/QMake/QtBaseConfig.pri)

TARGET_EXT = .plugin

# Set configuration of custom builds:
# ARX Compiler:
ARXC_OUTDIR = $$OUT_PWD/../$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET

INCLUDEPATH += $$OUT_PWD/../$$AUXINCLUDEPATH

#message(ARXC_OUTDIR $${ARXC_OUTDIR})

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)
include($(ACFDIR)/Config/QMake/CustomBuild.pri)


