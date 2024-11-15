#include "Network.h"
#include "Subscriber.h"

// ImtCore includes
#include <graphqlclient/graphqlclient.h>

// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>


int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	// QNetworkAccessManager manager;
	// QNetworkRequest request;
	// request.setUrl(QUrl("http://127.0.0.1:4444/Test/graphql"));
	// request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");
	// QByteArray data = "{\"query\":\"query TestHandler {TestHandler  {items {Id}}}\"}";
	// Network network;
	// QNetworkReply *reply = manager.post(request, data);
	// QObject::connect(reply, &QIODevice::readyRead, &network, &Network::slotReadyRead);
	// QObject::connect(reply, &QNetworkReply::errorOccurred,
	// 			&network, &Network::slotError);
	// QObject::connect(reply, &QNetworkReply::sslErrors,
	// 			&network, &Network::slotSslErrors);


	Subscriber subscriber;
	QString errorMessage;
	graphqlclient::ConnectionSettings connectionSettings;
	connectionSettings.productId ="Test";
	connectionSettings.webSocketServerUrl = QUrl("ws://127.0.0.1:4445");
	connectionSettings.serverUrl = QUrl("http://127.0.0.1:4444");

	Network network(connectionSettings);
	graphqlserver::ResultKeys resultKeys;
	resultKeys.AppendSimpleKey("currentData");
	network.SendRequest("TestHandler", graphqlserver::RequestType::RT_QUERY, QJsonObject(), resultKeys, errorMessage);


	graphqlclient::RegisterSubscription("OnCurrentData", resultKeys, subscriber, errorMessage);
	graphqlclient::ConnectToServer(connectionSettings, errorMessage);

	return app.exec();
}


