TARGET = imt3dgui

include($(ACFDIR)/Config/QMake/StaticConfig.pri)
include($(ACFDIR)/Config/QMake/QtGuiBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

#FORMS += $$_PRO_FILE_PWD_/../*.ui
RESOURCES += $$_PRO_FILE_PWD_/../*.qrc

INCLUDEPATH += ../../


