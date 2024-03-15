

win32*{
    INCLUDEPATH += $$PWD/../../3rdParty/openssl/1.1
    contains(QMAKE_HOST.arch, x86_64) {
		LIBS += -L$$PWD/../../3rdParty/openssl/1.1/lib/x64 -llibcrypto
	}
	else {
		LIBS += -L$$PWD/../../3rdParty/openssl/1.1/lib/x86 -llibcrypto
	}
}


mac*{
	INCLUDEPATH += $$PWD/../../3rdParty/openssl/1.1
	LIBS += -L$$PWD/../../3rdParty/openssl/1.1/lib/$$COMPILER_CODE -lcrypto -lssl
}


