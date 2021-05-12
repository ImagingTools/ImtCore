INCLUDEPATH += $$(PYBIND11DIR)/include
INCLUDEPATH += $$(PYTHON38DIR)/include

LIBS += $$(PYTHON38DIR)/libs/Python38.lib

QMAKE_CXXFLAGS += /bigobj


