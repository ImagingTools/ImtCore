project(imt3dgui LANGUAGES CXX)

#TARGET = imt3dgui

include($ENV{ACFCONFIGDIR}/CMake/StaticConfig.cmake)
#include($ENV{ACFCONFIGDIR}/CMake/QtGuiBaseConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)
include(../../../Config/CMake/ImtCore.cmake)

#include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
#include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

#FORMS += $$_PRO_FILE_PWD_/../*.ui
#RESOURCES += $$_PRO_FILE_PWD_/../*.qrc

#INCLUDEPATH += ../../

