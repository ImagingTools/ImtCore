INCLUDEPATH += $$PWD/../../../3rdParty/pybind11/include
INCLUDEPATH += $$(PYTHON38DIR)/include

LIBS += $$(PYTHON38DIR)/libs/Python38.lib

QMAKE_CXXFLAGS += /bigobj


