// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CQmlTestSetup.h"


// Qt includes
#include <QtQml/qqml.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtbase/Init.h>
#include <imtqml/CDataModelController.h>
#include <imtqml/CDocumentServiceController.h>
#include <imtqml/CFileIO.h>
#include <imtqml/CGqlModel.h>
#include <imtqml/CGqlRequest.h>
#include <imtqml/CNetworkEventInterceptor.h>
#include <imtqml/CQmlProcess.h>
#include <imtqml/CQmlWebSocket.h>
#include <imtqml/CRemoteFileController.h>


// Q_INIT_RESOURCE cannot be used inside of a namespace
static void InitQmlResources()
{
	Q_INIT_RESOURCE(imtstylecontrolsqml);
	DefaultImtCoreQmlInitializer::InitImtCoreSdl();
	DefaultImtCoreQmlInitializer::InitQml();
}


namespace imtqmltest
{


namespace
{


// Quick Test creates a new engine for every test file, a singleton instance would be bound to the first one
template <class Object>
void RegisterSingleton(const char* moduleName, const char* typeName, Object* objectPtr)
{
	qmlRegisterSingletonType<Object>(moduleName, 1, 0, typeName, [objectPtr](QQmlEngine*, QJSEngine*) -> QObject*{
		QQmlEngine::setObjectOwnership(objectPtr, QQmlEngine::CppOwnership);

		return objectPtr;
	});
}


} // namespace


// public methods

CQmlTestSetup::CQmlTestSetup()
	:m_networkAccessManagerFactory(m_transport)
{
}


// public slots

void CQmlTestSetup::applicationAvailable()
{
	InitQmlResources();

	qmlRegisterType<imtbase::CTreeItemModel>("com.imtcore.imtqml", 1, 0, "TreeItemModel");
	qmlRegisterType<imtqml::CQmlWebSocket>("com.imtcore.imtqml", 1, 0, "WebSocket");
	qmlRegisterType<imtqml::CGqlModel>("com.imtcore.imtqml", 1, 0, "GqlModel");
	qmlRegisterType<imtqml::CGqlRequest>("com.imtcore.imtqml", 1, 0, "GqlRequest");
	qmlRegisterType<imtqml::CRemoteFileController>("com.imtcore.imtqml", 1, 0, "RemoteFileController");
	qmlRegisterType<imtqml::FileIO>("com.imtcore.imtqml", 1, 0, "FileIO");
	qmlRegisterType<imtqml::CQmlProcess>("com.imtcore.imtqml", 1, 0, "Process");
	qmlRegisterType<imtqml::CDocumentServiceController>("com.imtcore.imtqml", 1, 0, "DocumentServiceController");
	qmlRegisterType<imtqml::CDataModelController>("com.imtcore.imtqml", 1, 0, "DataModelController");
	RegisterSingleton("com.imtcore.imtqml", "NetworkEventInterceptor", imtqml::CNetworkEventInterceptor::Instance());

	qmlRegisterModule("QtGraphicalEffects", 1, 12);
	qmlRegisterModule("QtGraphicalEffects", 1, 0);
	qmlRegisterModule("QtQuick.Dialogs", 1, 3);

	RegisterSingleton("imtqmltest", "GqlTestTransport", &m_transport);
}


void CQmlTestSetup::qmlEngineAvailable(QQmlEngine* enginePtr)
{
	enginePtr->addImportPath(QStringLiteral("qrc:/qml"));
	enginePtr->setNetworkAccessManagerFactory(&m_networkAccessManagerFactory);
}


} // namespace imtqmltest


