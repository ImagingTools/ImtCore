TARGET = imtgui

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

FORMS += $$files($$_PRO_FILE_PWD_/../*.ui, true)
RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, true)

