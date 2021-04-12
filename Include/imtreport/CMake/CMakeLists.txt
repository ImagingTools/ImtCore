project(imtreport LANGUAGES CXX)
#TARGET = imtreport

find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS PrintSupport REQUIRED)

include($ENV{ACFCONFIGDIR}/CMake/StaticConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

target_link_libraries(Qt5::PrintSupport)

#include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

