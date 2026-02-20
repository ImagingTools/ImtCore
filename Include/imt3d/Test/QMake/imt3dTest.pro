TARGET = imt3dTest

include($(ACFCONFIGDIR)/QMake/TestConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)

LIBS += -limt3d -li3d -li2d -limath -listd -liser -liimg -licalib -lidoc
