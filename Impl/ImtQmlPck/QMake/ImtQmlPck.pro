TARGET = ImtQmlPck

QT += qml quick quickwidgets

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limtqml -limtbase -limtgql

# Get build output directory of shadow build:
ACFSLNDIRBUILD = $$(ACFSLNDIR_BUILD)
!isEmpty(ACFSLNDIRBUILD){
	INCLUDEPATH += $$(ACFSLNDIR_BUILD)/$$AUXINCLUDEDIR
	LIBS += -L$$(ACFSLNDIR_BUILD)/Lib/$$COMPILER_DIR
}

# Get build output directory of shadow build:
ACFDIRBUILD = $$(ACFDIR_BUILD)
!isEmpty(ACFDIRBUILD){
	INCLUDEPATH += $$(ACFDIR_BUILD)/$$AUXINCLUDEDIR
	LIBS += -L$$(ACFDIR_BUILD)/Lib/$$COMPILER_DIR
}

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

# Set OS-specific build options:
win32-msvc*{
    # copying all Qt DLLs to destination directory
	greaterThan(QT_MAJOR_VERSION, 4): QMAKE_POST_LINK = $(QTDIR)\bin\windeployqt $$DESTDIR\ImtQmlPck.arp
}
