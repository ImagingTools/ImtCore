TARGET = imt3dTest

include($(ACFCONFIGDIR)/QMake/TestConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../../Config/QMake/ImtCore.pri)

LIBS += -limt3d -li3d -li2d -limath -licmm -liimg -licalib -lidoc -libase -liser -limod -listd
