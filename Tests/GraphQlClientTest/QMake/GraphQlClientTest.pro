TARGET = GraphQlClientTest

include($(ACFCONFIGDIR)/QMake/ApplicationConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR
LIBS += -limtbase -lgraphqlclient -lgraphqlserver

QT += core gui network

CONFIG += console

# Set OS-specific build options:
win32-msvc*{
	QMAKE_CXXFLAGS += /wd4264

	# copying all Qt DLLs to destination directory
	QMAKE_POST_LINK = $$(QTDIR)\bin\windeployqt $$DESTDIR\GraphQlClientTest.exe --verbose 2
}




