TARGET = ImtDeskGqlPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

LIBS += -L$(IMTCOREDIR)/Lib/$$COMPILER_DIR -limtbase -limtdesk -limtdeskgql -limtgql -limtdb -limtservergql -limtserverapp -limtauth -limtdoc -limtrest -limtdesksdl -limtbasesdl -limtcol

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
