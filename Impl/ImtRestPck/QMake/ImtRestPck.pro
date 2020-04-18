TARGET = ImtRestPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

CONFIG += network

LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtrest
LIBS += -L$$(ACFSLNDIR)/Lib/$$COMPILER_DIR -liproc

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

