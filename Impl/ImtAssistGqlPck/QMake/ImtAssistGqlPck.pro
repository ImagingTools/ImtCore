TARGET = ImtAssistGqlPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

LIBS += -L$(IMTCOREDIR)/Lib/$$COMPILER_DIR -limtbase -limtassist -limtassistgql -limtgql -limtdb -limtservergql -limtserverapp -limtauth -limtaccount -limtbasesdl -limtcol

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
