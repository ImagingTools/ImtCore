TARGET = imtrest

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

CONFIG += network

SOURCES += $$files($$_PRO_FILE_PWD_/../*.c, false)
RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)


