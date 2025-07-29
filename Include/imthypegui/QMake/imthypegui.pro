TARGET = imthypegui

include($(ACFDIR)/Config/QMake/StaticConfig.pri)
include($(ACFDIR)/Config/QMake/QtGuiBaseConfig.pri)
include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

FORMS += $$files($$_PRO_FILE_PWD_/../*.ui, false)
RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

