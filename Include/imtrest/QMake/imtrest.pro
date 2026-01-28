TARGET = imtrest

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

CONFIG += network
QT += websockets

LIBS += -L../../../Lib/$$COMPILER_DIR -limthttp

RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)


