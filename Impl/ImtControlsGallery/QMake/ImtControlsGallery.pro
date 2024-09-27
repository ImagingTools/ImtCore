TARGET = ImtControlsGallery

include($(ACFCONFIGDIR)/QMake/ApplicationConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

QT += xml network sql quick qml

QT += quickcontrols2

RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

LIBS += -L../../../Lib/$$COMPILER_DIR -liauth -limtauthdb -limtclientgql -limtauthgui -limtappsdl -limtauthsdl
LIBS += -limtbase -limtzip -limtrest  -limtstyle -limtqml -limtauth -limtgui
LIBS += -limtguigqlqml -limtauthguiqml -limtguiqml -limtcolguiqml
LIBS += -limtstylecontrolsqml -limtcontrolsqml -limtdocguiqml
LIBS += -limtlicgql -limtguigql -limtgql -limtauthgql -limtauthdb -limtcom -limtapp

# Set OS-specific build options:
win32-msvc*{
	QMAKE_CXXFLAGS += /wd4264

	# copying all Qt DLLs to destination directory
	greaterThan(QT_MAJOR_VERSION, 4): QMAKE_POST_LINK = set path=$(QTDIR)\bin;%path% && $(QTDIR)\bin\windeployqt --qmldir=$(LISADIR)/Impl/LisaQmlExe --qmldir=$(IMTCOREDIR)/Include/imtqml/Resources/qml --qmldir=$(IMTCOREDIR)/Include/imtgui/Qml/imtgui --qmldir=$(IMTCOREDIR)/Include/imtlicgui/Qml/imtlicgui --qmldir=$(LISADIR)/Include/lisaqml/Qml  $$DESTDIR
}

# Set configuration of custom builds:
# ARX Compiler:
ARXC_CONFIG = $$PWD/../../../Config/ImtCore.awc
ARXC_FILES += $$PWD/../ImtControlsGallery.acc
ARXC_OUTDIR = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET

# Conversion of resource templates:
win*{
    # File transformation
	# ACF_CONVERT_FILES = $$PWD/../VC/LisaQml.rc.xtracf
	# ACF_CONVERT_OUTDIR = $$AUXINCLUDEPATH/GeneratedFiles/$$TARGET
	# ACF_CONVERT_REGISTRY =  $$PWD/../VC/FileSubstitCopyApp.acc
	# ACF_CONVERT_CONFIG = $$PWD/../../../Config/BaseOnly.awc

#    RC_FILE = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET/LisaQml.rc
# RC_INCLUDEPATH = $$_PRO_FILE_PWD_
}

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)
include($(ACFDIR)/Config/QMake/CustomBuild.pri)


