TARGET = imtrest

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

CONFIG += network

SOURCES += $$_PRO_FILE_PWD_/../*.c

RESOURCES += $$_PRO_FILE_PWD_/../*.qrc

############### for mem test it'll be moved in right project later
#LIBS += -L../../../Lib/$$COMPILER_DIR -liauth -limeas -liqtmeas -liqtinsp -liproc -liinsp -liipr -liprocgui -lisig -liqtsig -licalibgui -licalib -licam -liqtcam  -lAcfLoc -liedge -liedgegui
LIBS += -limtauthgui
QT += qml


