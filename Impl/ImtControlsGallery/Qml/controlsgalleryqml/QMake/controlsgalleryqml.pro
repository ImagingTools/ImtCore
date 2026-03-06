TARGET = controlsgalleryqml

include($(ACFDIR)/Config/QMake/GeneralConfig.pri)
include($(IMTCOREDIR)/Config/QMake/QmlControls.pri)

buildwebdir = $$PWD/../../../../Bin/web

imtcoredir = $(IMTCOREDIR)

prepareWebQml($$buildwebdir)

# copy project qml from to
copyToWebDir($$PWD/../, $$buildwebdir/src)
copyToWebDir($$imtcoredir/Include/imtstylecontrolsqml/Qml/Fonts/, $$buildwebdir/Resources)
copyToWebDir($$imtcoredir/Include/imtstylecontrolsqml/Qml/Acf/, $$buildwebdir/src/Acf)

# copy translations
copyToWebDir($$PWD/../../../../Impl/ProLifeLoc/Translations/, $$buildwebdir/Resources/Translations)
copyToWebDir($$imtcoredir/Impl/ImtCoreLoc/Translations/, $$buildwebdir/Resources/Translations)

compyleWeb($$buildwebdir, "controlsgallery")

GENERATED_RESOURCES = $$_PRO_FILE_PWD_/../empty

include($(IMTCOREDIR)/Config/QMake/WebQrc.pri)

include($(ACFDIR)/Config/QMake/StaticConfig.pri)
DESTDIR = $$OUT_PWD/../../../../Lib/$$COMPILER_DIR

include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)
