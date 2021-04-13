project(ImtZipPck LANGUAGES CXX)
#TARGET = ImtZipPck


link_directories("$ENV{QUAZIP}/lib/${TARGETNAME}")

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)
#include(../../../../Config/CMake/Quazip.cmake)
#include(C:/Work/Rpos/ITDevelopment/Config/CMake/Quazip.cmake)

#include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
#include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
#include($(IMTROOTDIR)/Config/QMake/Quazip.pri)

#target_link_libraries(${PROJECT_NAME} imtzip)
#link_libraries("C:/Work/Rpos/ITDevelopment/ImtCore/Lib/DebugVC15_64/imtzip.lib")
#target_link_libraries(${PROJECT_NAME} "C:/Work/Rpos/ITDevelopment/ImtCore/Lib/DebugVC15_64/imtzip.lib")
target_link_libraries(${PROJECT_NAME} "C:/Work/Rpos/ITDevelopment/3rdParty/quazip/lib/DebugVC15_64/quazip.lib")
target_link_libraries(${PROJECT_NAME} "C:/Work/Rpos/AcfPublic/3rdParty/zlib/lib/DebugVC15_64/zlib.lib")

#LIBS += -L../../../Lib/$$COMPILER_DIR -limtzip
