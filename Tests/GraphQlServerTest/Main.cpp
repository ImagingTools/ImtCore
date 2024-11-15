#include "TestHandler.h"
#include "TestPublisher.h"

// ImtCore includes
#include <graphqlserver/graphqlserver.h>

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtCore/QJsonObject>




int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	TestHandler testHandler;
	TestPublisher testPublisher;
	QString errorMessage;
	graphqlserver::ServerSettings settings;
	settings.productId = "Test";
	settings.tcpPort = 4444;
	settings.webSocketPort = 4445;

	graphqlserver::RegisterGqlHandler("TestHandler", testHandler, errorMessage);
	graphqlserver::RegisterGqlPublisher("OnCurrentData", testPublisher, errorMessage);
	graphqlserver::StartServer(settings, errorMessage);

	return app.exec();
}


