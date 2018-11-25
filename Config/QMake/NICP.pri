INCLUDEPATH += $(NICPDIR)/include

LIBS += -L$(NICPDIR)/lib/$$COMPILER_DIR

CONFIG(release, debug|release){
	LIBS += -lnicp
}

CONFIG(debug, debug|release){
	LIBS += -lnicp_d
}

