#include "TestHandler.h"

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
	QString errorMessage;
	graphqlserver::ServerSettings settings;
	settings.ProductName = "Test";
	settings.tcpPort = 4444;
	settings.webSocketPort = 4445;

	graphqlserver::RegisterGqlHandler("TestHandler", testHandler, errorMessage);
	graphqlserver::InitServer(settings, errorMessage);
	graphqlserver::StartServer(errorMessage);

	return app.exec();
}


