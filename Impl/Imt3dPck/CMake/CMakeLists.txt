project(Imt3dPck LANGUAGES CXX)
#TARGET = Imt3dPck

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

#include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)


target_link_libraries(${PROJECT_NAME} imtbase imt3d imeas iinsp iproc imeas iinsp iproc icalib)
#LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limt3d

#LIBS += -L$$(ACFSLNDIR)/Lib/$$COMPILER_DIR -limeas -liinsp -liproc -licalib

include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)

#include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
#include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

