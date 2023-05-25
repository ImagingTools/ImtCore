include($(IACFDIR)/Config/QMake/zlib.pri)

INCLUDEPATH += $$PWD/../../3rdParty/quazip
LIBS += -L$(QUAZIP)/lib/$$COMPILER_DIR -lquazip

DEFINES += QUAZIP_STATIC

