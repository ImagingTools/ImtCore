TARGET = imtwidgets

include($(ACFDIR)/Config/QMake/StaticConfig.pri)
include($(ACFDIR)/Config/QMake/QtGuiBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

FORMS += $$_PRO_FILE_PWD_/../*.ui

