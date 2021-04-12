project(Imt3dViewPck LANGUAGES CXX)
#TARGET = Imt3dViewPck

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)

#include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
#include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
#include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

target_link_libraries(${PROJECT_NAME} imeas iinsp iipr -liproc imt3d imt3dview)
#LIBS += -L../../../Lib/$$COMPILER_DIR -limeas -liinsp -liipr -liproc
#LIBS += -limt3d -limt3dview

# Get build output directory of shadow build:
#ACFSLNDIRBUILD = $$(ACFSLNDIR_BUILD)
#!isEmpty(ACFSLNDIRBUILD){
#	INCLUDEPATH += $$(ACFSLNDIR_BUILD)/$$AUXINCLUDEDIR
#	LIBS += -L$$(ACFSLNDIR_BUILD)/Lib/$$COMPILER_DIR
#}

# Get build output directory of shadow build:
#ACFDIRBUILD = $$(ACFDIR_BUILD)
#!isEmpty(ACFDIRBUILD){
#	INCLUDEPATH += $$(ACFDIR_BUILD)/$$AUXINCLUDEDIR
#	LIBS += -L$$(ACFDIR_BUILD)/Lib/$$COMPILER_DIR
#}

