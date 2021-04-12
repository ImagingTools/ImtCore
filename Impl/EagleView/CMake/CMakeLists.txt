TARGET = EagleView

include($(ACFCONFIGDIR)/QMake/ApplicationConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

INCLUDEPATH += $(IMTCOREDIR)/Include
INCLUDEPATH += $(IMTCOREDIR)/Impl
INCLUDEPATH +=  $$AUXINCLUDEDIR

RESOURCES += $$_PRO_FILE_PWD_/../*.qrc

LIBS += -L../../../Lib/$$COMPILER_DIR -limeas -liproc -liqtmeas -lAcfSlnLoc -lAcfLoc
LIBS += -L../../../Lib/$$COMPILER_DIR -limtgui -limt3d -limt3dview -limt3dgui

HEADERS =

QT += xml


greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

# Set OS-specific build options:
win32-msvc*{
	QMAKE_CXXFLAGS += /wd4264

	# copying all Qt DLLs to destination directory
	greaterThan(QT_MAJOR_VERSION, 4): QMAKE_POST_LINK = set path=$(QTDIR)\bin;%path% && $(QTDIR)\bin\windeployqt $$DESTDIR
}

!macx-ios*{
	mac{
	#	ICON += $$PWD/../Mac/EagleView.icns
	#	QMAKE_INFO_PLIST = $$PWD/../Mac/Info.plist
	}
}

# Set configuration of custom builds:
# ARX Compiler:
ARXC_CONFIG = $$PWD/../../../Config/ImtCore.awc
ARXC_FILES += $$PWD/../EagleView.acc
ARXC_OUTDIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET

# Conversion of resource templates:
win*{
	# File transformation
	ACF_CONVERT_FILES = $$PWD/../VC/EagleView.rc.xtracf
	ACF_CONVERT_OUTDIR = $$AUXINCLUDEPATH/GeneratedFiles/$$TARGET
	ACF_CONVERT_REGISTRY =  $$PWD/../VC/FileSubstitCopyApp.acc
	ACF_CONVERT_CONFIG = $$PWD/../../../Config/BaseOnly.awc

	#RC_FILE = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET/EagleView.rc
	#RC_INCLUDEPATH = $$_PRO_FILE_PWD_
}

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)
include($(ACFCONFIGDIR)/QMake/CustomBuild.pri)


