include($(IACFDIR)/Config/QMake/zlib.pri)

INCLUDEPATH += $$PWD/../../3rdParty/quazip
win32*{
	LIBS += -L$(QUAZIP)/lib/$$COMPILER_DIR -lquazip
}

mac*{
	LIBS += -L$(QUAZIP)/lib/Qt6_$$COMPILER_CODE -lquazip
}

DEFINES += QUAZIP_STATIC

greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

