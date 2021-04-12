project(imtgui LANGUAGES CXX)
#TARGET = imtgui

include($ENV{ACFCONFIGDIR}/CMake/StaticConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)


#include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

#FORMS += $$_PRO_FILE_PWD_/../*.ui
#RESOURCES += $$_PRO_FILE_PWD_/../*.qrc

