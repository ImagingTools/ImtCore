TARGET = ImtAuthGqlPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

QT += qml

LIBS += -L$(IMTCOREDIR)/Lib/$$COMPILER_DIR -limtbase -limtlic -limtauthgql -limtauth -limtgql -limtdb -limtguigql -liauth -limtqml -limtrest -limtauthsdl -limtbasesdl -limtservergql -limtcol -limtclientgql -liqt -li2d

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

