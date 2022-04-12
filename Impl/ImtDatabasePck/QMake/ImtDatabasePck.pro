TARGET = ImtDatabasePck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtdb
LIBS += -L$$(ACFSLNDIR)/Lib/$$COMPILER_DIR

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

# Set OS-specific build options:
win32-msvc*{
        # copying all Qt DLLs to destination directory
	greaterThan(QT_MAJOR_VERSION, 4): QMAKE_POST_LINK = $(QTDIR)\bin\windeployqt $$DESTDIR\ImtDatabasePck.arp
}

