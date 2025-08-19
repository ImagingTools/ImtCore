TARGET = ImtClientGqlPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

CONFIG += network websockets
QT += websockets

LIBS += -L../../../Lib/$$COMPILER_DIR -limtbase -limtbasesdl -limtrest -limtgql -limtcom -limtclientgql -limtservergql
LIBS += -L$$(ACFSLNDIR)/Lib/$$COMPILER_DIR -liproc
LIBS += -L$$(ACFDIR)/Lib/$$COMPILER_DIR -liprm

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

# Set OS-specific build options:
win32-msvc*{
    # copying all Qt DLLs to destination directory
	greaterThan(QT_MAJOR_VERSION, 4): QMAKE_POST_LINK = $(QTDIR)\bin\windeployqt $$DESTDIR\ImtClientGqlPck.arp
}
