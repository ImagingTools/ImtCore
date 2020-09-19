INCLUDEPATH += $$PWD/../../../3rdParty/pybind11/include
INCLUDEPATH += $$(PYTHONDIR)/include

LIBS += $$(PYTHONDIR)/libs/Python37.lib

QMAKE_CXXFLAGS += /bigobj


