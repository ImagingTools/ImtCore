TARGET = ServerDemo

include($(ACFCONFIGDIR)/QMake/ApplicationConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

INCLUDEPATH += $(IMTCOREDIR)/Include
INCLUDEPATH += $(IMTCOREDIR)/Impl
INCLUDEPATH +=  $$AUXINCLUDEDIR

INCLUDEPATH += $$OUT_PWD/../../../Include
INCLUDEPATH += $$_PRO_FILE_PWD_/../../../Include

LIBS += -L$$OUT_PWD

LIBS += -L../../../../../Lib/$$COMPILER_DIR -limeas -liproc -lAcfSlnLoc -lAcfLoc
LIBS += -L../../../../../Lib/$$COMPILER_DIR -limtbase -limtrest -limtservletdemo

QT += xml
QT += network

# Set OS-specific build options:
win32-msvc*{
	QMAKE_CXXFLAGS += /wd4264

	# copying all Qt DLLs to destination directory
	greaterThan(QT_MAJOR_VERSION, 4): QMAKE_POST_LINK = set path=$(QTDIR)\bin;%path% && $$(QTDIR)\bin\windeployqt $$DESTDIR
}

# Set configuration of custom builds:
# ARX Compiler:
ARXC_CONFIG = $$PWD/../../../../../Config/ImtCore.awc
ARXC_FILES += $$PWD/../ServerDemo.acc
ARXC_OUTDIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
include($(ACFCONFIGDIR)/QMake/CustomBuild.pri)


