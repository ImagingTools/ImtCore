TARGET = imtstyleqml

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

