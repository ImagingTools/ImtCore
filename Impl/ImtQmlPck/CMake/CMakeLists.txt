project(ImtQmlPck LANGUAGES CXX)
#TARGET = ImtQmlPck

find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS QuickWidgets REQUIRED)

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

#include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
target_link_libraries(${PROJECT_NAME} Qt5::QuickWidgets)
target_link_libraries(${PROJECT_NAME} imtqml)
#LIBS += -L../../../Lib/$$COMPILER_DIR -limtqml

IF(WIN32)
	target_link_libraries(${PROJECT_NAME} User32 Advapi32)
ENDIF()

#win32
#{
#	LIBS += -lUser32 -lAdvapi32
#}

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

