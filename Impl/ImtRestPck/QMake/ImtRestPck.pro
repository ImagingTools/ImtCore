TARGET = ImtRestPck

include($(ACFDIR)/Config/QMake/ComponentConfig.pri)
include($(ACFDIR)/Config/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

CONFIG += network

LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtrest
LIBS += -L$$(ACFSLNDIR)/Lib/$$COMPILER_DIR -liproc

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)

