include($(ACFDIR)/Config/QMake/SharedLibraryConfig.pri)
include($(ACFDIR)/Config/QMake/QtBaseConfig.pri)

TARGET_EXT = .plugin

# Set configuration of custom builds:
# ARX Compiler:
ARXC_CONFIG = $(IMTCOREDIR)/Config/Acula.awc
ARXC_OUTDIR = $$OUT_PWD/../$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)
include($(ACFDIR)/Config/QMake/CustomBuild.pri)


