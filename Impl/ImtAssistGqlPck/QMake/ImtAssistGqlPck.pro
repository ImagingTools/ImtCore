TARGET = ImtAssistGqlPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

QT += sql
LIBS += -L$(IMTCOREDIR)/Lib/$$COMPILER_DIR -limtbase -limtassist -limtassistdb -limtassistgql -limtauthdb -limtgql -limtdb -limtservergql -limtserverapp -limtauth -limtaccount -limtbasesdl -limtcol

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
