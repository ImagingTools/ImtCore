TARGET = imtloggui

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
include($(ACULADIR)/Config/QMake/Acula.pri)

FORMS += $$_PRO_FILE_PWD_/../*.ui
RESOURCES += $$_PRO_FILE_PWD_/../*.qrc

