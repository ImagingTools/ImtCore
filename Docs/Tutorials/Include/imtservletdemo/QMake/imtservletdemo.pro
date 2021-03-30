TARGET = imtservletdemo

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

LIBS += -L../../../../../Lib/$$COMPILER_DIR -limeas -liproc -lAcfSlnLoc -lAcfLoc
LIBS += -L../../../../../Lib/$$COMPILER_DIR -limtbase -limtrest

CONFIG += network

