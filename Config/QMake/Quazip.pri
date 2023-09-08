include($(IACFDIR)/Config/QMake/zlib.pri)

INCLUDEPATH += $$PWD/../../3rdParty/quazip
LIBS += -L$(QUAZIP)/lib/$$COMPILER_DIR -lquazip

DEFINES += QUAZIP_STATIC

greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

