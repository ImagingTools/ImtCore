project(imtpy LANGUAGES CXX)
#TARGET = imtpy

link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../3rdParty/Python/3.8/libs)

include($ENV{ACFCONFIGDIR}/CMake/StaticConfig.cmake)
#include($ENV{ACFCONFIGDIR}/CMake/QtGuiBaseConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)
include(../../../Config/CMake/pybind11.cmake)

#target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/../../../../3rdParty/Python/3.8/libs/python38.lib)
#target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../../../3rdParty/Python/3.8/libs)

#target_link_libraries(${PROJECT_NAME} python38)

#include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
#include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
#include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
#include(../../../Config/QMake/pybind11.pri)

#INCLUDEPATH += ../../../3rdParty/ImtCore/Include
#INCLUDEPATH += ../../../3rdParty/ImtCore/Impl
#INCLUDEPATH += ../../../3rdParty/ImtCore/$$AUXINCLUDEDIR

#INCLUDEPATH += ../../../3rdParty/Acula/Include
#INCLUDEPATH += ../../../3rdParty/Acula/Impl
#INCLUDEPATH += ../../../3rdParty/Acula/$$AUXINCLUDEDIR

