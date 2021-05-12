
INCLUDEPATH += $(OPENSSLDIR)

win32: {
	contains(QMAKE_HOST.arch, x86_64) {
	LIBS += -L$(OPENSSLDIR)/lib/x64 -llibcrypto
    } else {
	LIBS += -L$(OPENSSLDIR)/lib/x86 -llibcrypto
    }
}

unix: LIBS += -lcrypto
