TARGET = EagleLoc

include($(ACFDIR)/Config/QMake/StaticConfig.pri)

HEADERS =
SOURCES =
RESOURCES += $$_PRO_FILE_PWD_/../*.qrc

TRANSLATIONS += $$_PRO_FILE_PWD_/../Translations/*.ts

# Set configuration of custom builds:
# ARX Compiler:
ARXC_CONFIG = $$PWD/../../../Config/Eagle.awc
ARXC_FILES += $$PWD/../*.acc
ARXC_OUTDIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET



include($(ACFDIR)/Config/QMake/CustomBuild.pri)
