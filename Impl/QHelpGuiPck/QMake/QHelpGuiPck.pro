TARGET = QHelpGuiPck

include($(ACFDIR)/Config/QMake/ComponentConfig.pri)
include($(ACFDIR)/Config/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -lqhelpgui

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)

