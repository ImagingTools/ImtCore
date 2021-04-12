project(imtzip LANGUAGES CXX)

#TARGET = imtzip

link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../3rdParty/quazip/lib/${TARGETNAME})

include($ENV{ACFCONFIGDIR}/CMake/StaticConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)
include(../../../../Config/CMake/Quazip.cmake)

#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
#include($(IMTROOTDIR)/Config/QMake/Quazip.pri)

