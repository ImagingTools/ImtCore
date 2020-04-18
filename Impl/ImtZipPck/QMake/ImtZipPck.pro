TARGET = ImtZipPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
include($(IMTROOTDIR)/Config/QMake/Quazip.pri)


LIBS += -L../../../Lib/$$COMPILER_DIR -limtzip
