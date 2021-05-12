TARGET = imtpy

include($$PWD/../../../Config/QMake/ImtCore.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(IMTCOREDIR)/Config/QMake/pybind11.pri)
i
INCLUDEPATH += ../../../3rdParty/ImtCore/Include
INCLUDEPATH += ../../../3rdParty/ImtCore/Impl
INCLUDEPATH += ../../../3rdParty/ImtCore/$$AUXINCLUDEDIR

INCLUDEPATH += ../../../3rdParty/Acula/Include
INCLUDEPATH += ../../../3rdParty/Acula/Impl
INCLUDEPATH += ../../../3rdParty/Acula/$$AUXINCLUDEDIR

