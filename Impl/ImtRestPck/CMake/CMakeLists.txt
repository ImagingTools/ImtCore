project(ImtRestPck LANGUAGES CXX)
#TARGET = ImtRestPck
message("QT_DEFAULT_MAJOR_VERSION " ${QT_DEFAULT_MAJOR_VERSION})

find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS Core Network REQUIRED)

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)
#include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

#CONFIG += network

target_link_libraries(${PROJECT_NAME} imtbase imtrest iproc)

#target_link_directories(${PROJECT_NAME} PRIVATE $ENV{ACFSLNDIR}/Lib/${TARGETNAME})
#target_link_libraries(${PROJECT_NAME} iproc)
#target_link_directories(${PROJECT_NAME} PRIVATE $ENV{ACFDIR}/Lib/${TARGETNAME})

#LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtrest
#LIBS += -L$$(ACFSLNDIR)/Lib/$$COMPILER_DIR -liproc


include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)


target_link_libraries(${PROJECT_NAME} Qt5::Network )

#include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
#include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

