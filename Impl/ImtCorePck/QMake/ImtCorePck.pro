TARGET = ImtCorePck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

LIBS += -lidoc -liser -limeas -licomm
LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase
LIBS += -L$$(ACFDIR)/Lib/$$COMPILER_DIR -liprm

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

