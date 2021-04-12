include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)
include($ENV{IACFDIR}/Config/CMake/Iacf.cmake)

##include_directories("$ENV{IMTCOREDIR}/$$AUXINCLUDEDIR")
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
#include($(IACFDIR)/Config/QMake/Iacf.pri)

#INCLUDEPATH += $(IMTCOREDIR)/Include
#INCLUDEPATH += $(IMTCOREDIR)/Impl
#INCLUDEPATH += $(IMTCOREDIR)/$$AUXINCLUDEDIR
#LIBS += -L$$PWD/../../Lib/$$COMPILER_DIR


#win32{
target_link_libraries(${PROJECT_NAME} advapi32 user32)
#	LIBS += -ladvapi32 -luser32
#}


