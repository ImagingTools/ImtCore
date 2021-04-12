project(ImtPyPck LANGUAGES CXX)
#TARGET = ImtPyPck

link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../3rdParty/Python/3.8/libs)

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)
include(../../../Config/CMake/pybind11.cmake)

#include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
#include(../../../Config/QMake/pybind11.pri)

include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)

#include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
#include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

target_link_libraries(${PROJECT_NAME} imtpy)
#LIBS += -L../../../Lib/$$COMPILER_DIR -limtpy


