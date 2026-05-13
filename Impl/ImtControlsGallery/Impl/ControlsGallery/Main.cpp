// Qt includes
#include <QtQuick/QQuickWindow>

// ACF includes
#include <ibase/IApplication.h>

// ImtCore includes
#include <imtbase/Init.h>
#include <GeneratedFiles/ControlsGallery/CControlsGallery.h>


// -----------------------------------------------------------------------------
// GraphQL ApiClient wiring for QML data controllers
// -----------------------------------------------------------------------------
//
// The QML controllers from the imtqml module
// (GqlBasedCollectionDataController.qml, GqlDocumentDataController.qml in
// Qml/imtguigql) need an IGqlClient instance to send GraphQL
// requests to the server. In the C++ world IGqlClient is provided by
// imtclientgql::CApiClientComp; in QML the same instance is exposed as the
// singleton "GqlClientBridge" (com.imtcore.imtqml 1.0).
//
// Wiring is done declaratively in ControlsGallery.acc:
//
//     <Element Id="GraphQLClientEngine"
//              PackageId="ImtClientGqlPck" ComponentId="GraphQLClientEngine">
//         <!-- ServerConnectionInterface = LocalSettings/ServerConnectionInterfaceParam -->
//         <!-- ApplicationInfo           = ClientApplication/ApplicationInfo          -->
//     </Element>
//     <Element Id="ApiClient"
//              PackageId="ImtClientGqlPck" ComponentId="ApiClient">
//         <!-- ProtocolEngine = GraphQLClientEngine -->
//     </Element>
//     <Element Id="GqlClientBridge"
//              PackageId="ImtQmlPck"        ComponentId="GqlClientBridge">
//         <!-- ApiClient = ApiClient -->
//     </Element>
//
// "GqlClientBridge" is added to ClientApplication/ComponentsToInitialize so
// that icomp constructs it (and resolves the IGqlClient I_REF) before the QML
// engine starts. After construction CGqlClientBridge::Instance() returns the
// live bridge, and CStaticQmlTypeRegistratorComp publishes it to QML as the
// singleton "GqlClientBridge" (resolved lazily through a callback). QML then
// uses it without any extra setup:
//
//     import com.imtcore.imtqml 1.0
//     GqlBasedCollectionDataController {
//         apiClient   : GqlClientBridge   // optional - defaults to the singleton
//         collectionId: "MyCollection"
//     }
// -----------------------------------------------------------------------------



int main(int argc, char *argv[])
{
#ifdef WEB_COMPILE
	Q_INIT_RESOURCE(controlsgalleryqmlWeb);
#endif
	Q_INIT_RESOURCE(controlsgalleryqml);
	Q_INIT_RESOURCE(controlsgalleryContactInfosSdl);
	Q_INIT_RESOURCE(controlsgalleryContactInfoCollectionDocumentManagerSdl);
	Q_INIT_RESOURCE(imtguivars);

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
	QQuickWindow::setGraphicsApi(QSGRendererInterface::Direct3D12);
#endif
#endif

	return Run<CControlsGallery, DefaultImtCoreQmlInitializer>(argc, argv);
}


