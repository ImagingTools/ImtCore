include($(ACFDIR)/Config/QMake/GeneralConfig.pri)
include($(IMTCOREDIR)/Config/QMake/WebCompiler.pri)

defineTest(prepareWebQml) {
	buildwebdir = $$1
	imtcoredir = $(IMTCOREDIR)

	copyToWebDir($$imtcoredir/Tools/JQML/v2/core/dist, $$buildwebdir/Resources)

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
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseSearchSdl, $$buildwebdir/src/imtbaseSearchSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseCommandsSdl, $$buildwebdir/src/imtbaseCommandsSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseSettingsSdl, $$buildwebdir/src/imtbaseSettingsSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseCollectionDocumentServiceSdl, $$buildwebdir/src/imtbaseCollectionDocumentServiceSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseCollectionImportSdl, $$buildwebdir/src/imtbaseCollectionImportSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseUndoManagerSdl, $$buildwebdir/src/imtbaseUndoManagerSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseProgressManagerSdl, $$buildwebdir/src/imtbaseProgressManagerSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseDocumentIdFilterSdl, $$buildwebdir/src/imtbaseDocumentIdFilterSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseMimeTypeSdl, $$buildwebdir/src/imtbaseMimeTypeSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseFilterableSelectSdl, $$buildwebdir/src/imtbaseFilterableSelectSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imt2dsdl/SDL/1.0/QML/imt2dGeometrySdl, $$buildwebdir/src/imt2dGeometrySdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imt2dsdl/SDL/1.0/QML/imt2dPaintSystemSdl, $$buildwebdir/src/imt2dPaintSystemSdl)

	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthGroupsSdl, $$buildwebdir/src/imtauthGroupsSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthRolesSdl, $$buildwebdir/src/imtauthRolesSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthUsersSdl, $$buildwebdir/src/imtauthUsersSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthAuthorizationSdl, $$buildwebdir/src/imtauthAuthorizationSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthAuthorizationOptionsSdl, $$buildwebdir/src/imtauthAuthorizationOptionsSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthProfileSdl, $$buildwebdir/src/imtauthProfileSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthSessionsSdl, $$buildwebdir/src/imtauthSessionsSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthPersonalAccessTokensSdl, $$buildwebdir/src/imtauthPersonalAccessTokensSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthTenantsSdl, $$buildwebdir/src/imtauthTenantsSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthTenantCollectionDocumentServiceSdl, $$buildwebdir/src/imtauthTenantCollectionDocumentServiceSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthTenantMembershipsSdl, $$buildwebdir/src/imtauthTenantMembershipsSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthRelationshipCollectionDocumentServiceSdl, $$buildwebdir/src/imtauthRelationshipCollectionDocumentServiceSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthCrossOrgGrantCollectionDocumentServiceSdl, $$buildwebdir/src/imtauthCrossOrgGrantCollectionDocumentServiceSdl)

	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtlicsdl/SDL/1.0/QML/imtlicFeaturesSdl, $$buildwebdir/src/imtlicFeaturesSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtlicsdl/SDL/1.0/QML/imtlicProductsSdl, $$buildwebdir/src/imtlicProductsSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtlicsdl/SDL/1.0/QML/imtlicLicensesSdl, $$buildwebdir/src/imtlicLicensesSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtappsdl/SDL/1.0/QML/imtappApplicationSdl, $$buildwebdir/src/imtappApplicationSdl)

	copyToWebDir($$imtcoredir/Qml/imtdeskgui, $$buildwebdir/src/imtdeskgui)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtdesksdl/SDL/1.0/QML/imtdeskImtDeskSdl, $$buildwebdir/src/imtdeskImtDeskSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtdesksdl/SDL/1.0/QML/imtdeskTicketCollectionDocumentServiceSdl, $$buildwebdir/src/imtdeskTicketCollectionDocumentServiceSdl)

	copyToWebDir($$imtcoredir/Qml/imtchatgui, $$buildwebdir/src/imtchatgui)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtchatsdl/SDL/1.0/QML/imtchatImtChatSdl, $$buildwebdir/src/imtchatImtChatSdl)
	copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/imtchatsdl/SDL/1.0/QML/imtchatConversationCollectionDocumentServiceSdl, $$buildwebdir/src/imtchatConversationCollectionDocumentServiceSdl)
}
