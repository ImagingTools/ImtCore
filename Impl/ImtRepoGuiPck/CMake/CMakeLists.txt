project(ImtRepoGuiPck LANGUAGES CXX)
#TARGET = ImtRepoGuiPck

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

#include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

target_link_libraries(${PROJECT_NAME} imtbase imtgui imtrepo imtrepogui)
#LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtgui -limtrepo -limtrepogui

include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)

#include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
#include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

