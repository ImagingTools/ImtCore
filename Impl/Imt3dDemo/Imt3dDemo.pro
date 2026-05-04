TARGET = Imt3dDemo
TEMPLATE = app

QT += core gui qml quick shadertools

CONFIG += c++17

SOURCES += main.cpp

RESOURCES += imt3ddemo.qrc

INCLUDEPATH += ../../Include
LIBS += -L$$OUT_PWD/../../lib -limt3dgui -limt3dview
