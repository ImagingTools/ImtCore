TARGET = ImtDevPck

include($(ACFDIR)/Config/QMake/ComponentConfig.pri)
include($(ACFDIR)/Config/QMake/QtBaseConfig.pri)

LIBS += -L$$(ACFSLNDIR)/Lib/$$COMPILER_DIR -limeas -liproc
LIBS += -L../../../Lib/$$COMPILER_DIR -limtdev -limtbase

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)

