TARGET = imtwidgets

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

FORMS += $$files($$_PRO_FILE_PWD_/../*.ui, false)

