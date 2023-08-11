TARGET = ImtControlsGallery

include($(ACFDIR)/Config/QMake/ApplicationConfig.pri)
include($(ACFDIR)/Config/QMake/QtBaseConfig.pri)
include($(IMTCOREDIR)/Config/QMake/OpenSSL.pri)


HEADERS =
QT += xml network sql quick qml

RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)

LIBS += -L../../../Lib/$$COMPILER_DIR -liauth -liqtgui -liservice
LIBS += -limtbase -limtgui -limtauth -limtauthgui -limtlicdb -limtlic -limtlicgui -lImtCoreLoc -limtwidgets -limtzip -limtrest -limtcrypt -limt3dgui -limtrepo -limtstyle -limtstyleqml -limtqml -limtdb -limtfile
LIBS += -limtlicgql -limtguigql -limtgql -limtauthgql -limtauthdb -limtcom -limtapp
LIBS += -lImtCoreLoc

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
#win*{
#    # File transformation
#	ACF_CONVERT_FILES = $$PWD/../VC/ImtControlsGallery.rc.xtracf
#	ACF_CONVERT_OUTDIR = $$AUXINCLUDEPATH/GeneratedFiles/$$TARGET
#	ACF_CONVERT_REGISTRY =  $$PWD/../VC/FileSubstitCopyApp.acc
#	ACF_CONVERT_CONFIG = $$PWD/../../../Config/BaseOnly.awc

#    RC_FILE = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET/ImtControlsGallery.rc
#	RC_INCLUDEPATH = $$_PRO_FILE_PWD_
#}

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)
include($(ACFDIR)/Config/QMake/CustomBuild.pri)
include($(IMTCOREDIR)/Config/QMake/WebCompiler.pri)

buildwebdir = $$PWD/../../../Bin/web

imtcoredir = $(IMTCOREDIR)

copyToWebDir($$imtcoredir/3rdParty/JQML/Core/dist, $$buildwebdir/Resources)

# copy base qml sources from to
copyToWebDir($$imtcoredir/Include/imtstyleqml/Qml/Acf, $$buildwebdir/src/Acf)
copyToWebDir($$imtcoredir/Include/imtgui/Qml/imtcontrols, $$buildwebdir/src/imtcontrols)
copyToWebDir($$imtcoredir/Include/imtqml/Qml/imtqml, $$buildwebdir/src/imtqml)
copyToWebDir($$imtcoredir/Include/imtauthgui/Qml/imtauthgui, $$buildwebdir/src/imtauthgui)

# copy acula qml from to
copyToWebDir($$PWD/../Qml, $$buildwebdir/src)
copyToWebDir($$PWD/../Resources/html, $$buildwebdir/Resources)
copyToWebDir($$PWD/../Resources/dist, $$buildwebdir/Resources)
copyToWebDir($$PWD/../Resources/Translations, $$buildwebdir/Resources/Translations)

compyleWeb($$buildwebdir, "gallery")

#GENERATED_RESOURCES = $$_PRO_FILE_PWD_/../empty

include($(IMTCOREDIR)/Config/QMake/WebQrc.pri)
include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)



