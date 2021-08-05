TARGET = ImtComPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

CONFIG += network websockets

LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase
LIBS += -L$$(ACFSLNDIR)/Lib/$$COMPILER_DIR -limtcom

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

