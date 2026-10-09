// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/CHttpTransport.h>

#ifdef IMTREST_HTTP_SERVER_AVAILABLE


// STL includes
#include <map>

// Qt includes
#include <QtCore/QAtomicInt>
#include <QtCore/QHash>
#include <QtCore/QPair>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QSslConfiguration>
#include <QtHttpServer/QAbstractHttpServer>
#include <QtHttpServer/QHttpServerResponder>


class QTimer;


namespace imtrest
{


/**
	Internal connection queue of CHttpServerShard.
	QAbstractHttpServer can only take over connections from a listening QTcpServer (see QAbstractHttpServer::bind),
	but the connections of the transport are accepted by the single CHttpListener and assigned to the I/O threads.
	This queue listens on a loopback port with paused accepting and is used only to pass already created sockets
	to QtHttpServer through QTcpServer::addPendingConnection. Any real connection to the loopback port is rejected.
*/
class CHttpConnectionQueue: public QTcpServer
{
public:
	explicit CHttpConnectionQueue(QObject* parentPtr = nullptr);

	bool Open();
	void Enqueue(QTcpSocket* socketPtr);

protected:
	// reimplemented (QTcpServer)
	virtual void incomingConnection(qintptr socketDescriptor) override;
};


/**
	QtHttpServer instance serving the connections of one I/O thread of CHttpTransport.
	The object lives and works exclusively in its I/O thread.
*/
class CHttpServerShard: public QAbstractHttpServer
{
public:
	CHttpServerShard(
				CHttpTransport& transport,
				IHttpTransportHandler& handler,
				const CHttpTransport::Configuration& configuration);
	~CHttpServerShard() override;

	bool Initialize();

	/**
		Number of connections served by this shard (including reserved ones). Thread-safe.
	*/
	int GetConnectionCount() const;

	/**
		Reserve a slot for a new connection. Thread-safe. Must be followed by \c AcceptSocket.
	*/
	void ReserveConnection();

	void AcceptSocket(qintptr socketDescriptor, bool isSecure, const QSslConfiguration& sslConfiguration);
	void SendResponse(const QByteArray& requestId, const IHttpTransportHandler::ResponseData& response);
	void Shutdown();

protected:
	// reimplemented (QAbstractHttpServer)
	virtual bool handleRequest(const QHttpServerRequest& request, QHttpServerResponder& responder) override;
	virtual void missingHandler(const QHttpServerRequest& request, QHttpServerResponder& responder) override;

private:
	enum ConnectionState
	{
		/**
			Waiting for the headers of the first request to decide between HTTP and WebSocket.
		*/
		CS_CLASSIFYING,

		/**
			The connection is served by QtHttpServer.
		*/
		CS_HTTP
	};

	struct Connection
	{
		QTcpSocket* socketPtr = nullptr;
		QTimer* timerPtr = nullptr;
		ConnectionState state = CS_CLASSIFYING;
		bool isSecure = false;
		bool isHttp10 = false;
		int pendingRequestCount = 0;
		QHostAddress peerAddress;
		quint16 peerPort = 0;
	};

	struct PendingResponse
	{
		PendingResponse(QHttpServerResponder&& responderRef, QTcpSocket* socket, bool isCloseRequested, bool isKeepAliveRequested)
			:responder(std::move(responderRef)),
			socketPtr(socket),
			closeConnection(isCloseRequested),
			keepAliveRequested(isKeepAliveRequested)
		{
		}

		QHttpServerResponder responder;
		QTcpSocket* socketPtr;
		bool closeConnection;
		bool keepAliveRequested;
	};

	typedef QPair<QHostAddress, quint16> PeerKey;

	void OnSocketReadyRead(QTcpSocket* socketPtr);
	void OnSocketDisconnected(QTcpSocket* socketPtr);
	void OnConnectionTimeout(QTcpSocket* socketPtr);

	void ClassifyConnection(Connection& connection);
	void HandOverToHttp(Connection& connection);
	void HandOverToWebSocket(Connection& connection);
	void RestartTimer(Connection& connection);
	void ForgetConnection(QTcpSocket* socketPtr);
	void DropPendingResponses(QTcpSocket* socketPtr);
	void FinishResponse(const QByteArray& requestId, const IHttpTransportHandler::ResponseData& response);
	void ReleaseConnectionSlot();

	static bool IsWebSocketUpgrade(const QByteArray& headerData);
	static QByteArray GetMethodName(const QHttpServerRequest& request);

private:
	CHttpTransport& m_transport;
	IHttpTransportHandler& m_handler;
	CHttpTransport::Configuration m_configuration;

	CHttpConnectionQueue* m_connectionQueuePtr;

	QHash<QTcpSocket*, Connection> m_connections;
	QHash<PeerKey, QTcpSocket*> m_peerSockets;
	std::map<QByteArray, PendingResponse> m_pendingResponses;

	QAtomicInt m_connectionCount;
};


} // namespace imtrest


#endif // IMTREST_HTTP_SERVER_AVAILABLE


