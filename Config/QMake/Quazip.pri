include($(IACFDIR)/Config/QMake/zlib.pri)

INCLUDEPATH += $(QUAZIP)/include

LIBS += -L$(QUAZIP)/lib/$$COMPILER_DIR -lquazip

DEFINES += QUAZIP_STATIC

