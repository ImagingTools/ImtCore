project(imtzip LANGUAGES CXX)

#TARGET = imtzip

link_directories("$ENV{QUAZIP}/lib/${TARGETNAME}")

include($ENV{ACFCONFIGDIR}/CMake/StaticConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)
#include(../../../../Config/CMake/Quazip.cmake)
include(C:/Work/Rpos/ITDevelopment/Config/CMake/Quazip.cmake)

#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
#include($(IMTROOTDIR)/Config/QMake/Quazip.pri)

target_link_libraries(${PROJECT_NAME} "C:/Work/Rpos/ITDevelopment/3rdParty/quazip/lib/DebugVC15_64/quazip.lib")
target_link_libraries(${PROJECT_NAME} "C:/Work/Rpos/AcfPublic/3rdParty/zlib/lib/DebugVC15_64/zlib.lib")