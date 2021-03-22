TARGET = ImtAuthGuiPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtgui -limtauth -limtauthgui -limtrepogui

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

