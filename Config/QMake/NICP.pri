INCLUDEPATH += $(NICP_DIR)/include

LIBS += -L$(NICP_DIR)/lib/$$COMPILER_DIR

CONFIG(release, debug|release){
	LIBS += -lnicp
}

CONFIG(debug, debug|release){
	LIBS += -lnicp_d
}

