include($(IACFDIR)/Config/QMake/zlib.pri)

INCLUDEPATH += $$PWD/../../3rdParty/quazip/include

LIBS += -L$$PWD/../../3rdParty/quazip/lib/$$COMPILER_DIR -lquazip

DEFINES += QUAZIP_STATIC

