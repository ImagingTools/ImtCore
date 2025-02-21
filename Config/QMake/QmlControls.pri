include($(ACFDIR)/Config/QMake/GeneralConfig.pri)
include($(IMTCOREDIR)/Config/QMake/WebCompiler.pri)

defineTest(prepareWebQml) {
	buildwebdir = $$1
	imtcoredir = $(IMTCOREDIR)

	copyToWebDir($$imtcoredir/3rdParty/JQML2/Core/dist, $$buildwebdir/Resources)

# copy project qml from to
#	copyToWebDir($$PWD/../Qml, $$buildwebdir/src)
#	copyToWebDir($$PWD/../Resources/html, $$buildwebdir/Resources)
#	copyToWebDir($$PWD/../Resources/Translations, $$buildwebdir/Resources/Translations)

# copy base qml sources from to
	copyToWebDir($$imtcoredir/Qml/web, $$buildwebdir/Resources)

	copyToWebDir($$imtcoredir/Qml/imtgui, $$buildwebdir/src/imtgui)
	copyToWebDir($$imtcoredir/Qml/imtguigql, $$buildwebdir/src/imtguigql)
	copyToWebDir($$imtcoredir/Qml/imtauthgui, $$buildwebdir/src/imtauthgui)
	copyToWebDir($$imtcoredir/Qml/imtlicgui, $$buildwebdir/src/imtlicgui)
	copyToWebDir($$imtcoredir/Qml/imtcolgui, $$buildwebdir/src/imtcolgui)
	copyToWebDir($$imtcoredir/Qml/imtdocgui, $$buildwebdir/src/imtdocgui)
	copyToWebDir($$imtcoredir/Qml/imtcontrols, $$buildwebdir/src/imtcontrols)

	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseComplexCollectionFilterSdl, $$buildwebdir/src/imtbaseComplexCollectionFilterSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseImtBaseTypesSdl, $$buildwebdir/src/imtbaseImtBaseTypesSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseImtCollectionSdl, $$buildwebdir/src/imtbaseImtCollectionSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseDocumentRevisionSdl, $$buildwebdir/src/imtbaseDocumentRevisionSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseDocumentCollectionFilterSdl, $$buildwebdir/src/imtbaseDocumentCollectionFilterSdl)

	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthGroupsSdl, $$buildwebdir/src/imtauthGroupsSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthRolesSdl, $$buildwebdir/src/imtauthRolesSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthUsersSdl, $$buildwebdir/src/imtauthUsersSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthAuthorizationSdl, $$buildwebdir/src/imtauthAuthorizationSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthAuthorizationOptionsSdl, $$buildwebdir/src/imtauthAuthorizationOptionsSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthProfileSdl, $$buildwebdir/src/imtauthProfileSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthSessionsSdl, $$buildwebdir/src/imtauthSessionsSdl)
	
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtlicsdl/SDL/1.0/QML/imtlicFeaturesSdl, $$buildwebdir/src/imtlicFeaturesSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtlicsdl/SDL/1.0/QML/imtlicProductsSdl, $$buildwebdir/src/imtlicProductsSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtlicsdl/SDL/1.0/QML/imtlicLicensesSdl, $$buildwebdir/src/imtlicLicensesSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtappsdl/SDL/1.0/QML/imtappApplicationSdl, $$buildwebdir/src/imtappApplicationSdl)
}
