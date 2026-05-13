INCLUDEPATH += $$PWD/../../3rdParty/pybind11/include

isEmpty(PYTHON38DIR) {
    PYTHON38DIR = $$(PYTHON38DIR)
}

isEmpty(PYTHON38DIR) {
    error("PYTHON38DIR environment variable is not set. Please set it to your Python 3.8 installation directory.")
}

INCLUDEPATH += $$PYTHON38DIR/include

LIBS += $$PYTHON38DIR/libs/Python38.lib

QMAKE_CXXFLAGS += /bigobj


