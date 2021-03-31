TARGET = ServletDemoPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtservletdemo
LIBS += -L$$(ACFSLNDIR)/Lib/$$COMPILER_DIR -limeas -liinsp -liproc

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

