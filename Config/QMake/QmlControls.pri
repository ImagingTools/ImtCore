include($(IMTCOREDIR)/Config/QMake/WebCompiler.pri)

defineTest(prepareWebQml) {
	buildwebdir = $$1
	imtcoredir = $(IMTCOREDIR)

	copyToWebDir($$imtcoredir/3rdParty/JQML2/Core/dist/, $$buildwebdir/Resources)

# copy project qml from to
#	copyToWebDir($$PWD/../Qml/, $$buildwebdir/src)
#	copyToWebDir($$PWD/../Resources/html/, $$buildwebdir/Resources)
#	copyToWebDir($$PWD/../Resources/Translations/, $$buildwebdir/Resources/Translations)

# copy base qml sources from to
	copyToWebDir($$imtcoredir/Qml/web/, $$buildwebdir/Resources)
	copyToWebDir($$imtcoredir/Qml/imtgui/, $$buildwebdir/src/imtgui)
	copyToWebDir($$imtcoredir/Qml/imtguigql/, $$buildwebdir/src/imtguigql)
	copyToWebDir($$imtcoredir/Qml/imtauthgui/, $$buildwebdir/src/imtauthgui)
	copyToWebDir($$imtcoredir/Qml/imtlicgui/, $$buildwebdir/src/imtlicgui)
	copyToWebDir($$imtcoredir/Qml/imtcolgui/, $$buildwebdir/src/imtcolgui)
	copyToWebDir($$imtcoredir/Qml/imtdocgui/, $$buildwebdir/src/imtdocgui)
	copyToWebDir($$imtcoredir/Qml/imtcontrols/, $$buildwebdir/src/imtcontrols)
}

