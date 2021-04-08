TARGET = ImtCryptPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtcrypt

win32: {
		contains(QMAKE_HOST.arch, x86_64) {
		LIBS += -L../../../../3rdParty/openssl/1.1/lib/x64 -llibcrypto
	} else {
		LIBS += -L../../../../3rdParty/openssl/1.1/lib/x86 -llibcrypto
	}
}

unix: LIBS += -lcrypto

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

INCLUDEPATH += ../../../../3rdParty/openssl/1.1
