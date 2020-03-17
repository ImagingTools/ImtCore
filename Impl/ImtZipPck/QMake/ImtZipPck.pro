TARGET = ImtZipPck

include($(ACFDIR)/Config/QMake/ComponentConfig.pri)
include($(ACFDIR)/Config/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)
include($(IMTROOTDIR)/Config/QMake/Quazip.pri)


LIBS += -L../../../Lib/$$COMPILER_DIR -limtzip
