TARGET = imtrest


include($(ACFDIR)/Config/QMake/StaticConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

CONFIG += network

SOURCES += $$_PRO_FILE_PWD_/../*.c

