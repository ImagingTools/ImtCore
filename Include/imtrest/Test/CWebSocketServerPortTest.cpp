// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CWebSocketServerPortTest.h"


// Qt includes
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TSimComponentWrap.h>

// ImtCore includes
#include <imtcom/CServerConnectionInterfaceParamComp.h>
#include <imtrest/CWebSocketProtocolEngineComp.h>
#include <imtrest/CWebSocketServerComp.h>


namespace
{


typedef icomp::TSimSharedComponentPtr<imtrest::CWebSocketProtocolEngineComp> Engine;
// A model, as the package of the component makes it: the server follows the changes of its port
typedef icomp::TSimSharedComponentPtr<icomp::TModelCompWrap<imtcom::CServerConnectionInterfaceParamComp>> Connection;
typedef icomp::TSimComponentWrap<imtrest::CWebSocketServerComp> WebSocketServer;


int FindFreePort()
{
	QTcpServer server;
	if (!server.listen(QHostAddress::LocalHost, 0)){
		return 0;
	}

	return server.serverPort();
}


bool IsAccepted(int port)
{
	QTcpSocket socket;
	socket.connectToHost(QHostAddress::LocalHost, quint16(port));

	return socket.waitForConnected(5000);
}


/**
	The server of the real components, created as the container of the components does: the attributes and the references are set, then
	the components are created. The members are destroyed in the reverse order, so the server goes first.
*/
struct SServer
{
	SServer()
	{
		port = FindFreePort();

		engine->InitComponent();

		// The HTTP port is of another server, as in an application with both servers
		connection->SetIntAttr("DefaultHttpPort", FindFreePort());
		connection->SetIntAttr("DefaultWebSocketPort", port);
		connection->InitComponent();

		server.SetRef("ProtocolEngine", engine);
		server.SetRef("WebServerConnectionInterface", connection);
		server.InitComponent();
	}

	bool IsListening() const
	{
		const imtrest::IServer* serverPtr = &server;

		return (port != 0) && (serverPtr->GetServerStatus() == imtrest::IServer::SS_LISTENING);
	}

	int port = 0;
	Engine engine;
	Connection connection;
	WebSocketServer server;
};


} // namespace


// A new WebSocket port of the connection interface moves the running server: the old port is closed, the new one accepts
void CWebSocketServerPortTest::PortChangeMovesServerTest()
{
	SServer server;
	QVERIFY(server.IsListening());
	QVERIFY(IsAccepted(server.port));

	const int newPort = FindFreePort();
	QVERIFY(newPort != 0 && newPort != server.port);
	server.connection->SetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET, newPort);

	QVERIFY(server.IsListening());
	QVERIFY(IsAccepted(newPort));
	QVERIFY(!IsAccepted(server.port));
}


// A port that another program listens on cannot be taken: the server stays on its port
void CWebSocketServerPortTest::BusyPortKeepsServerTest()
{
	SServer server;
	QVERIFY(server.IsListening());

	QTcpServer otherProgram;
	QVERIFY(otherProgram.listen(QHostAddress::Any, 0));
	server.connection->SetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET, otherProgram.serverPort());

	QVERIFY(server.IsListening());
	QVERIFY(IsAccepted(server.port));
}


// The HTTP port is not the port of the WebSocket server: its change leaves the server and its connections alone
void CWebSocketServerPortTest::OtherPortChangeKeepsServerTest()
{
	SServer server;
	QVERIFY(server.IsListening());

	QTcpSocket client;
	client.connectToHost(QHostAddress::LocalHost, quint16(server.port));
	QVERIFY(client.waitForConnected(5000));

	server.connection->SetPort(imtcom::IServerConnectionInterface::PT_HTTP, FindFreePort());

	QVERIFY(server.IsListening());
	QVERIFY(IsAccepted(server.port));
	QVERIFY(!client.waitForDisconnected(300));
}


I_ADD_TEST(CWebSocketServerPortTest);


