TARGET = DocumentServiceIntegrationTestPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($$PWD/../../../../Config/QMake/ImtCore.pri)

# The package is located one directory deeper than the standard packages
DESTDIR = $$OUT_PWD/../../../../Bin/$$COMPILER_DIR

LIBS += -limtdoc

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
