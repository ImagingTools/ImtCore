TARGET = imt3dguiqml

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

QT += qml quick quick3d

RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)
