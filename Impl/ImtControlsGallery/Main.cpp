// Qt includes
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>
#include <QQuickStyle>

// ACF includes
#include <ibase/IApplication.h>

// ImtCore includes
#include <imtstyle/CImtStyle.h>
#include <imtbase/CTreeItemModel.h>
#include <imtqml/CGqlModel.h>
#include <imtqml/CRemoteFileController.h>
#include <imtqml/CQuickApplicationComp.h>
#include <imtqml/CGqlRequest.h>
#include <GeneratedFiles/ImtControlsGallery/CImtControlsGallery.h>


int main(int argc, char *argv[])
{
#ifdef WEB_COMPILE
	Q_INIT_RESOURCE(ImtControlsGalleryWeb);
#endif

	Q_INIT_RESOURCE(imtguigqlqml);
	Q_INIT_RESOURCE(imtstyle);
	Q_INIT_RESOURCE(imtstylecontrolsqml);
	Q_INIT_RESOURCE(imtcontrolsqml);
	Q_INIT_RESOURCE(imtguiqml);
	Q_INIT_RESOURCE(imtcolguiqml);
	Q_INIT_RESOURCE(imtdocguiqml);
	Q_INIT_RESOURCE(imtauthguiqml);

	Q_INIT_RESOURCE(imtresthtml);
	Q_INIT_RESOURCE(ImtControlsGallery);
	Q_INIT_RESOURCE(imtauthguiTheme);
	Q_INIT_RESOURCE(imtguiTheme);

	Q_INIT_RESOURCE(imtguivars);
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

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	
	CImtControlsGallery instance;

	qmlRegisterType<imtbase::CTreeItemModel>("Acf", 1, 0, "TreeItemModel");
	qmlRegisterType<imtqml::CGqlModel>("Acf", 1, 0, "GqlModel");
	qmlRegisterType<imtqml::CRemoteFileController>("Acf", 1, 0, "RemoteFileController");
	qmlRegisterType<imtqml::CGqlRequest>("Acf", 1, 0, "GqlRequest");

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	qmlRegisterModule("QtGraphicalEffects", 1, 12);
	qmlRegisterModule("QtGraphicalEffects", 1, 0);
	qmlRegisterModule("QtQuick.Dialogs", 1, 3);
#else
	qmlRegisterModule("QtQuick.Dialogs", 6, 2);
	qmlRegisterModule("Qt5Compat.GraphicalEffects", 6, 0);
#endif

	QQuickStyle::setStyle("Material");

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr != nullptr){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}


