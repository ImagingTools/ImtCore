TARGET = imtqml

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

QT += quick quickwidgets

#FORMS += $$_PRO_FILE_PWD_/../*.ui
RESOURCES += $$_PRO_FILE_PWD_/../*.qrc

