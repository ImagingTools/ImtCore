TARGET = ImtZipPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
include($(IMTCOREDIR)/Config/QMake/Quazip.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limtzip

