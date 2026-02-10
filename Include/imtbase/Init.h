// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDirIterator>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>
#include <QtQml/QQmlEngine>

// ACF includes
#include <ibase/IApplication.h>

// ImtCore includes
#include <imtstyle/CImtStyle.h>


class DefaultImtCoreQmlInitializer
{
public:
	static void Init()
	{
		InitImtCoreLocalization();
		InitImtCoreResources();
		InitStyle();
		InitTheme();
		InitImtCoreSdl();
		InitQml();
	}

	static void InitAcfLocalization()
	{
		Q_INIT_RESOURCE(AcfLoc);
		Q_INIT_RESOURCE(AcfSlnLoc);
	}

	static void InitImtCoreLocalization()
	{
		InitAcfLocalization();
		Q_INIT_RESOURCE(ImtCoreLoc);
	}

	static void InitImtCoreSdl()
	{
		Q_INIT_RESOURCE(imtbaseSearchSdl);
		Q_INIT_RESOURCE(imtbaseCommandsSdl);
		Q_INIT_RESOURCE(imtbaseSettingsSdl);
		Q_INIT_RESOURCE(imtbaseImtBaseTypesSdl);
		Q_INIT_RESOURCE(imtbaseImtCollectionSdl);
		Q_INIT_RESOURCE(imtbaseDocumentCollectionFilterSdl);
		Q_INIT_RESOURCE(imtbaseComplexCollectionFilterSdl);
		Q_INIT_RESOURCE(imtbaseDocumentRevisionSdl);
		Q_INIT_RESOURCE(imtauthGroupsSdl);
		Q_INIT_RESOURCE(imtauthRolesSdl);
		Q_INIT_RESOURCE(imtauthUsersSdl);
		Q_INIT_RESOURCE(imtauthProfileSdl);
		Q_INIT_RESOURCE(imtauthSessionsSdl);
		Q_INIT_RESOURCE(imtauthAuthorizationOptionsSdl);
		Q_INIT_RESOURCE(imtauthAuthorizationSdl);
		Q_INIT_RESOURCE(imtappApplicationSdl);
		Q_INIT_RESOURCE(imtcolorLabSdl);
		Q_INIT_RESOURCE(imtcolorRgbColorHexSdl);
		Q_INIT_RESOURCE(imtbaseCollectionDocumentManagerSdl);
		Q_INIT_RESOURCE(imtbaseUndoManagerSdl);
		Q_INIT_RESOURCE(imtbaseDocumentIdFilterSdl);
		Q_INIT_RESOURCE(imtbaseProgressManagerSdl);
		Q_INIT_RESOURCE(imtbaseCollectionImportSdl);
		Q_INIT_RESOURCE(imt2dGeometrySdl);
		Q_INIT_RESOURCE(imt2dPaintSystemSdl);
		Q_INIT_RESOURCE(imtauthPersonalAccessTokensSdl);
	}

	static void InitQml()
	{
		Q_INIT_RESOURCE(imtcontrolsqml);
		Q_INIT_RESOURCE(imtguiqml);
		Q_INIT_RESOURCE(imtcolguiqml);
		Q_INIT_RESOURCE(imtdocguiqml);
		Q_INIT_RESOURCE(imtguigqlqml);
		Q_INIT_RESOURCE(imtauthguiqml);
		Q_INIT_RESOURCE(imtlicguiqml);
	}

	static void InitTheme()
	{
		Q_INIT_RESOURCE(imtguiTheme);
		Q_INIT_RESOURCE(imtauthguiTheme);
	}

	static void InitImtCoreResources()
	{
		Q_INIT_RESOURCE(imtbase);
		Q_INIT_RESOURCE(imtstylecontrolsqml);

		Q_INIT_RESOURCE(imtauthguilight);
		Q_INIT_RESOURCE(imtauthguidark);

		Q_INIT_RESOURCE(imtlicguilight);
		Q_INIT_RESOURCE(imtlicguidark);

		Q_INIT_RESOURCE(imtgui);
		Q_INIT_RESOURCE(imtguilight);
		Q_INIT_RESOURCE(imtguidark);

		Q_INIT_RESOURCE(imtstyle);
		Q_INIT_RESOURCE(imtstylelight);
		Q_INIT_RESOURCE(imtstyledark);

		Q_INIT_RESOURCE(imtdb);
	}

	static void InitStyle()
	{
		imtstyle::CImtStyle* imtStylePtr = imtstyle::CImtStyle::GetInstance();
		Q_ASSERT(imtStylePtr != nullptr);
		imtStylePtr->SetStyleType(imtstyle::CImtStyle::ST_IMAGINGTOOLS);
	
		imtStylePtr->setBaseStyle(QStyleFactory::create("fusion"));
		QApplication::setStyle(imtStylePtr);

		QCoreApplication* appPtr = QCoreApplication::instance();
		if (appPtr != nullptr){
			appPtr->setProperty("ThemeId", "Light");
		}
	}
};

template <class ApplicationComposite, class AppInitializer, bool autoInit = false>
int Run(int argc, char *argv[])
{
	AppInitializer::Init();

	QDirIterator dirIter(":/Style");
	while (dirIter.hasNext()){
		qDebug() << dirIter.filePath();

		dirIter.next();
	}

	QDirIterator dirIter3(":/Icons");
	while (dirIter3.hasNext()){
		qDebug() << dirIter3.filePath();

		dirIter3.next();
	}

	QDirIterator dirIter2(":/Styles");
	while (dirIter2.hasNext()){
		qDebug() << dirIter2.filePath();

		dirIter2.next();
	}

	ApplicationComposite instance;

	if (autoInit){
		instance.EnsureAutoInitComponentsCreated();
	}

	ibase::IApplication* applicationPtr = instance.template GetInterface<ibase::IApplication>();
	if (applicationPtr != nullptr){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}


