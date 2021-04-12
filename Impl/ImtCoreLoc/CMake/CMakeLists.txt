project(ImtCoreLoc LANGUAGES CXX)
#TARGET = ImtCoreLoc

include($ENV{ACFCONFIGDIR}/CMake/StaticConfig.cmake)
include($ENV{ACFCONFIGDIR}/CMake/CustomBuild.cmake)


#include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)

#HEADERS =
#SOURCES =
#RESOURCES += $$_PRO_FILE_PWD_/../*.qrc

#TRANSLATIONS += $$_PRO_FILE_PWD_/../Translations/*.ts

# Set configuration of custom builds:
# ARX Compiler:
#ARXC_CONFIG = $$PWD/../../../Config/ImtCore.awc
#ARXC_FILES += $$PWD/../*.acc
#ARXC_OUTDIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET

#include($(ACFCONFIGDIR)/QMake/CustomBuild.pri)

