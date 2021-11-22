TARGET = ImtGuiGqlPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limeas -liproc -liauth
LIBS += -limtgui -limtgql -limtguigql -limtloggui -limtbase -limtwidgets -li2d -liqt2d -limtstyle


include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

