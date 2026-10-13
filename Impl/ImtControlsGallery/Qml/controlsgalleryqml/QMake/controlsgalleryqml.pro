TARGET = controlsgalleryqml

include($(ACFDIR)/Config/QMake/GeneralConfig.pri)
include($(IMTCOREDIR)/Config/QMake/WebCompiler.pri)

buildwebdir = $$PWD/../../../../Bin/web

imtcoredir = $$(IMTCOREDIR)

# compile web application with the JQML v3 compiler, QML sources are taken from the directories listed in gallery.json
jqCompileWeb($$buildwebdir, $$PWD/../gallery.json, $$PWD/../ImtControlsGalleryWeb.qml, "/ControlsGallery/Views/", "../Icons/GalleryIcon.svg", $$imtcoredir/Impl/ImtCoreLoc/Translations)

GENERATED_RESOURCES = $$_PRO_FILE_PWD_/../empty

include($(IMTCOREDIR)/Config/QMake/WebQrc.pri)

include($(ACFDIR)/Config/QMake/StaticConfig.pri)
DESTDIR = $$OUT_PWD/../../../../Lib/$$COMPILER_DIR

include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

RESOURCES += $$files($$_PRO_FILE_PWD_/../*.qrc, false)
