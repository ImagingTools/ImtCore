TARGET = imtgql

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

QT += network websockets qml quick quickwidgets sql

LIBS += -L../../../Lib/$$COMPILER_DIR -limtcom

