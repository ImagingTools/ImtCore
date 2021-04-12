project(ImtLogGuiPck LANGUAGES CXX)
#TARGET = ImtLogGuiPck

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

#include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

target_link_libraries(${PROJECT_NAME} imeas iinsp iipr iproc icalib imtgui imtlog imtloggui imtbase imtwidgets i2d iqt2d iedge)
#LIBS += -L../../../Lib/$$COMPILER_DIR -limeas -liinsp -liipr -liproc -licalib
#LIBS += -limtgui -limtlog -limtloggui -limtbase -limtwidgets -li2d -liqt2d -liedge

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

include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)

#include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
#include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

