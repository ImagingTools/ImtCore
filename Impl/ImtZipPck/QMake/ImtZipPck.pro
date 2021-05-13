TARGET = ImtZipPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)
include($$PWD/../../../Config/QMake/Quazip.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limtzip

