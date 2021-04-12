project(ImtCorePck LANGUAGES CXX)
#TARGET = ImtCorePck

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

#include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

target_link_libraries(${PROJECT_NAME} imtbase imeas iinsp iproc)
#Qt5::Xmlpatterns
#LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase
#LIBS += -L$$(ACFSLNDIR)/Lib/$$COMPILER_DIR -limeas -liinsp -liproc


include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)
#include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
#include($(ACFCONFIGDIR)/QMake/AcfStd.pri)




