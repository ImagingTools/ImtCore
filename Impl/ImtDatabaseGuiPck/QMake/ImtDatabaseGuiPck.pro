TARGET = ImtDatabaseGuiPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtgui --limtdbgui -limtwidgets -limtstyle
LIBS += -L$$(ACFSLNDIR)/Lib/$$COMPILER_DIR

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

