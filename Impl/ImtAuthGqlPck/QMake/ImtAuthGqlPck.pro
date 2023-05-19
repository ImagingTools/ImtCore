TARGET = ImtAuthGqlPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtlic -limtauthgql -limtauth -limtgql -limtdb -limtguigql

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

