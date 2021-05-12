
INCLUDEPATH += $$PWD/../../3rdParty/openssl/1.1

win32:
{
	contains(QMAKE_HOST.arch, x86_64) {
		LIBS += -L$$PWD/../../3rdParty/openssl/1.1/lib/x64 -llibcrypto
	}
	else {
		LIBS += -L$$PWD/../../3rdParty/openssl/1.1/lib/x86 -llibcrypto
	}
}

unix: LIBS += -lcrypto

