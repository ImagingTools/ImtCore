project(ImtReportPck LANGUAGES CXX)
#TARGET = ImtReportPck

#find_package(Qt5::PrintSupport REQUIRED)
find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS PrintSupport REQUIRED)


include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

#include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

target_link_libraries(${PROJECT_NAME} imtreport imtbase)
#LIBS += -L../../../Lib/$$COMPILER_DIR -limtreport -limtbase

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

target_link_libraries(Qt5::PrintSupport)

#include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
#include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

