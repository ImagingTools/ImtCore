project(ImtAuthPck LANGUAGES CXX)
#TARGET = ImtAuthPck

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)


target_link_libraries(${PROJECT_NAME} imtbase imtauth)
#LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtauth


include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)

#include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
#include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

