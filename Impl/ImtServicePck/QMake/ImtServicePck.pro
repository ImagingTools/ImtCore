TARGET = ImtServicePck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

CONFIG += network websockets

LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtcom
LIBS += -L$$(ACFSLNDIR)/Lib/$$COMPILER_DIR -limtservice

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

