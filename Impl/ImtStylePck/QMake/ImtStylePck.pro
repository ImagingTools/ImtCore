TARGET = ImtStylePck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

