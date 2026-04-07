TARGET = ImtChatGqlPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

LIBS += -L$(IMTCOREDIR)/Lib/$$COMPILER_DIR -limtbase -limtchat -limtchatgql -limtgql -limtdb -limtservergql -limtchatsdl -limtbasesdl -limtcol

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
