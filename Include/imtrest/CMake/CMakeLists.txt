project(imtrest)
#TARGET = imtrest


include($ENV{ACFCONFIGDIR}/CMake/StaticConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS Core Network REQUIRED)

#include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

#CONFIG += network

#SOURCES += $$_PRO_FILE_PWD_/../*.c

