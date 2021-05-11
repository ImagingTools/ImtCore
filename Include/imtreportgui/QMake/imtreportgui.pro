TARGET = imtreportgui

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

QT += printsupport

FORMS += $$files($$_PRO_FILE_PWD_/../*.ui, false)
#RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

