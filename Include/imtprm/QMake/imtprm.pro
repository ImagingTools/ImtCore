TARGET = imtprm

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

SOURCES += $$files($$_PRO_FILE_PWD_/../*.cpp, false)


