TARGET = ImtGqlPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

CONFIG += network websockets

LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtgql
LIBS += -L$$(ACFSLNDIR)/Lib/$$COMPILER_DIR -liproc

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

# Set OS-specific build options:
win32-msvc*{
    # copying all Qt DLLs to destination directory
	greaterThan(QT_MAJOR_VERSION, 4): QMAKE_POST_LINK = $(QTDIR)\bin\windeployqt $$DESTDIR\ImtGqlPck.arp
}
