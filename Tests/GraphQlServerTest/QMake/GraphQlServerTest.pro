TARGET = LisaServer

include($(LISADIR)/Config/QMake/Lisa.pri)

copyToDestDir($$PWD/../../../Build/Migrations, $$PWD/../../../Bin/$$COMPILER_DIR/Migrations)
copyToDestDir($(IMTCOREDIR)/Install/Database/LogMigrations, $$PWD/../../../Bin/$$COMPILER_DIR/LogMigrations)

# Set OS-specific build options:
win32-msvc*{
	QMAKE_CXXFLAGS += /wd4264

	# copying all Qt DLLs to destination directory
	greaterThan(QT_MAJOR_VERSION, 4): QMAKE_POST_LINK += set path=$(QTDIR)\bin;%path% && $(QTDIR)\bin\windeployqt --qmldir=$(LISADIR)/Impl/LisaServer  --qmldir=$(IMTCOREDIR)/Qml --qmldir=$(LISADIR)/Include/lisaqml/Qml  $$DESTDIR/LisaServer.exe
}

# Set configuration of custom builds:
# ARX Compiler:
ARXC_CONFIG = $$PWD/../../../Config/Lisa.awc
ARXC_FILES += $$PWD/../LisaServer.acc
ARXC_OUTDIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET

# Conversion of resource templates:
win*{
# File transformation
	# ACF_CONVERT_FILES = $$PWD/../VC/LisaServer.rc.xtracf
	ACF_CONVERT_OUTDIR = $$AUXINCLUDEPATH/GeneratedFiles/$$TARGET
	ACF_CONVERT_REGISTRY =  $$PWD/../VC/FileSubstitCopyApp.acc
	ACF_CONVERT_CONFIG = $$PWD/../../../Config/BaseOnly.awc

# RC_FILE = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET/LisaServer.rc
   RC_INCLUDEPATH = $$_PRO_FILE_PWD_
}

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)
include($(ACFDIR)/Config/QMake/CustomBuild.pri)


