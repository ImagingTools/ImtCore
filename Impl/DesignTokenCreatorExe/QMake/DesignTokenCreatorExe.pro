TARGET = DesignTokenCreatorExe

include($(ACFCONFIGDIR)/QMake/ApplicationConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

LIBS += -L../../../Lib/$$COMPILER_DIR
LIBS +=  -limtbase -limtstyle

QT += core gui

# Set OS-specific build options:
win32-msvc*{
	QMAKE_CXXFLAGS += /wd4264

	# copying all Qt DLLs to destination directory
	greaterThan(QT_MAJOR_VERSION, 4): QMAKE_POST_LINK = set path=$$(QTDIR)\bin;%path% && $$(QTDIR)\bin\windeployqt $$DESTDIR
}


# Set configuration of custom builds:
# ARX Compiler:
ARXC_CONFIG = $$PWD/../../../Config/ImtCore.awc
ARXC_FILES += $$PWD/../DesignTokenCreatorExe.acc
ARXC_OUTDIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET

# Conversion of resource templates:
win*{
	# File transformation
	ACF_CONVERT_OUTDIR = $$AUXINCLUDEPATH/GeneratedFiles/$$TARGET
	ACF_CONVERT_REGISTRY =  $$PWD/../VC/FileSubstitCopyApp.acc
	ACF_CONVERT_CONFIG = $$PWD/../../../Config/BaseOnly.awc
}

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)
include($(ACFDIR)/Config/QMake/CustomBuild.pri)

