TARGET = ImtCryptPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
include($(IMTCOREDIR)/Config/QMake/OpenSSL.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtcrypt

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

