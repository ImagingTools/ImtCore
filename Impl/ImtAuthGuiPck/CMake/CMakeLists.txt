project(ImtAuthGuiPck LANGUAGES CXX)
#TARGET = ImtAuthGuiPck

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtgui imtauth imtauthgui imtrepogui)
#LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtgui -limtauth -limtauthgui -limtrepogui


include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)

#include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
#include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

