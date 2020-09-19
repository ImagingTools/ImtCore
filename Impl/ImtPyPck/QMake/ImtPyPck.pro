TARGET = ImtPyPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
include(../../../Config/QMake/pybind11.pri)

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limtpy


