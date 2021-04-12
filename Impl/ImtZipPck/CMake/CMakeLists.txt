project(ImtZipPck LANGUAGES CXX)
#TARGET = ImtZipPck

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)


#include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
#include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
#include($(IMTROOTDIR)/Config/QMake/Quazip.pri)

target_link_libraries(${PROJECT_NAME} imtzip)
#LIBS += -L../../../Lib/$$COMPILER_DIR -limtzip
