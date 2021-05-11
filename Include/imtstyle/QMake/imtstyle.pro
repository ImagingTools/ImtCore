TARGET = imtstyle

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)
