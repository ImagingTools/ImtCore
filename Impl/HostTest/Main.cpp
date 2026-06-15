// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
//
// HostTest — minimal QML host application for exploring ImtCore components.
//
// Initialises all SDL-generated QML modules and all QML libraries from
// ImtCore/Qml, registers C++ types for com.imtcore.imtqml 1.0, adds
// "qrc:/qml" as a QML import path, and loads Main.qml.
//
// After startup every module from ImtCore/Qml (imtcontrols, imtgui, imtgui-
// gql, imtauthgui, imtlicgui, imtcolgui, imtdocgui, imtchatgui, imtdeskgui)
// and every SDL-generated QML module is importable from Main.qml.
//

#include <QtWidgets/QApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/qqml.h>
#include <QtCore/QUrl>
#include <QtCore/QString>

// C++ types registered under com.imtcore.imtqml 1.0
#include <imtbase/CTreeItemModel.h>
#include <imtqml/CQmlWebSocket.h>
#include <imtqml/CGqlModel.h>
#include <imtqml/CGqlRequest.h>
#include <imtqml/CRemoteFileController.h>
#include <imtqml/CFileIO.h>
#include <imtqml/CQmlProcess.h>
#include <imtqml/CNetworkEventInterceptor.h>
#include <imtqml/CDocumentServiceController.h>
#include <imtqml/CDataModelController.h>

// ImtCore — initialises resources, style, SDL modules and QML modules
#include <imtbase/Init.h>


int main(int argc, char *argv[])
{
	// Register the application's own QML resource (Main.qml)
	Q_INIT_RESOURCE(hosttest);

	QApplication app(argc, argv);
	app.setApplicationName("HostTest");
	app.setOrganizationName("ImagingTools");

	// Initialise all ImtCore resources, style, theme, core SDL modules and
	// QML modules (imtcontrolsqml, imtguiqml, imtauthguiqml, etc.)
	DefaultImtCoreQmlInitializer::Init();

	// Initialise SDL modules not covered by DefaultImtCoreQmlInitializer
	Q_INIT_RESOURCE(imtbaseMimeTypeSdl);
	Q_INIT_RESOURCE(imtauthUserActionsSdl);
	Q_INIT_RESOURCE(imtchatImtChatSdl);
	Q_INIT_RESOURCE(imtchatConversationCollectionDocumentServiceSdl);
	Q_INIT_RESOURCE(imtdeskImtDeskSdl);
	Q_INIT_RESOURCE(imtdeskTicketCollectionDocumentServiceSdl);
	Q_INIT_RESOURCE(imtlicProductsSdl);
	Q_INIT_RESOURCE(imtlicLicensesSdl);
	Q_INIT_RESOURCE(imtlicFeaturesSdl);
	Q_INIT_RESOURCE(imtpayImtPaySdl);

	// Initialise QML modules not covered by DefaultImtCoreQmlInitializer
	Q_INIT_RESOURCE(imtchatguiqml);
	Q_INIT_RESOURCE(imtdeskguiqml);

	// ----------------------------------------------------------------
	// Register C++ types for the "com.imtcore.imtqml 1.0" QML module.
	// These are needed so that any imtcontrols/imtgui component that
	// does  `import com.imtcore.imtqml 1.0`  can instantiate the types.
	// ----------------------------------------------------------------
	qmlRegisterType<imtbase::CTreeItemModel>       ("com.imtcore.imtqml", 1, 0, "TreeItemModel");
	qmlRegisterType<imtqml::CQmlWebSocket>         ("com.imtcore.imtqml", 1, 0, "WebSocket");
	qmlRegisterType<imtqml::CGqlModel>             ("com.imtcore.imtqml", 1, 0, "GqlModel");
	qmlRegisterType<imtqml::CGqlRequest>           ("com.imtcore.imtqml", 1, 0, "GqlRequest");
	qmlRegisterType<imtqml::CRemoteFileController> ("com.imtcore.imtqml", 1, 0, "RemoteFileController");
	qmlRegisterType<imtqml::FileIO>                ("com.imtcore.imtqml", 1, 0, "FileIO");
	qmlRegisterType<imtqml::CQmlProcess>           ("com.imtcore.imtqml", 1, 0, "Process");
	qmlRegisterSingletonInstance<imtqml::CNetworkEventInterceptor>(
		"com.imtcore.imtqml", 1, 0, "NetworkEventInterceptor",
		imtqml::CNetworkEventInterceptor::Instance());
	qmlRegisterType<imtqml::CDocumentServiceController>("com.imtcore.imtqml", 1, 0, "DocumentServiceController");
	qmlRegisterType<imtqml::CDataModelController>  ("com.imtcore.imtqml", 1, 0, "DataModelController");

	// Register compatibility stubs for Qt-version-dependent modules so that
	// QML files importing e.g. QtGraphicalEffects (Qt5 style) load cleanly.
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	qmlRegisterModule("QtGraphicalEffects", 1, 12);
	qmlRegisterModule("QtGraphicalEffects", 1, 0);
	qmlRegisterModule("QtQuick.Dialogs", 1, 3);
#else
	qmlRegisterModule("QtQuick.Dialogs", 6, 2);
	qmlRegisterModule("Qt5Compat.GraphicalEffects", 6, 0);
#endif

	QQmlApplicationEngine engine;

	// All QML modules (ImtCore/Qml and SDL-generated) embed their files
	// under the "/qml" resource prefix — registering it as an import path
	// makes every module importable by name inside QML files.
	engine.addImportPath(QStringLiteral("qrc:/qml"));

	engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));

	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}
