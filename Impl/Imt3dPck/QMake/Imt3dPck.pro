TARGET = Imt3dPck


include($(ACFDIR)/Config/QMake/ComponentConfig.pri)
include($(ACFDIR)/Config/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limeas -liinsp -liipr -liproc -licalib
LIBS += -limt3d -limtbase

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)

