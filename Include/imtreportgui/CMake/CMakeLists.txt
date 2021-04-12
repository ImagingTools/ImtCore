project(imtreportgui LANGUAGES CXX)
#TARGET = imtreportgui
find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS PrintSupport REQUIRED)

include($ENV{ACFCONFIGDIR}/CMake/StaticConfig.cmake)
#include($ENV{ACFCONFIGDIR}/CMake/QtGuiBaseConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

target_link_libraries(Qt5::PrintSupport)
#include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

#QT += printsupport

#FORMS += $$_PRO_FILE_PWD_/../*.ui
#RESOURCES += $$_PRO_FILE_PWD_/../*.qrc

