project(ImtLicensePck LANGUAGES CXX)
#TARGET = ImtLicensePck

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

#include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)


#LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtlic

target_link_libraries(${PROJECT_NAME} imtbase imtlic)

include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)

#include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
#include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

