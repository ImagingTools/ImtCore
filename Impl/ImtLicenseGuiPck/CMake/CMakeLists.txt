project(ImtLicenseGuiPck LANGUAGES CXX)
#TARGET = ImtLicenseGuiPck


include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

#include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

target_link_libraries(${PROJECT_NAME} imeas iproc iauth iedge imtgui imtloggui imtbase imtwidgets imtlic imtlicgui imtrepogui)

#LIBS += -L../../../Lib/$$COMPILER_DIR -limeas -liproc -liauth -liedge
#LIBS += -limtgui -limtloggui -limtbase -limtwidgets -limtlic -limtlicgui -limtrepogui


include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)

#include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
#include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

