TARGET = ImtCryptPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
include($(IMTROOTDIR)/Config/QMake/OpenSSL.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtcrypt


include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

INCLUDEPATH += ../../../../3rdParty/openssl/1.1
