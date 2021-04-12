project(imtqml LANGUAGES CXX)
#TARGET = imtqml

find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS QuickWidgets REQUIRED)

include($ENV{ACFCONFIGDIR}/CMake/StaticConfig.cmake)
#include($ENV{ACFCONFIGDIR}/CMake/QtGuiBaseConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

target_link_libraries(${PROJECT_NAME} Qt5::QuickWidgets)

#include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

#QT += qml quick quickwidgets

#FORMS += $$_PRO_FILE_PWD_/../*.ui
#RESOURCES += $$_PRO_FILE_PWD_/../*.qrc

