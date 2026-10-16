// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QtGlobal>

/**
	The HTTP transport is based on QtHttpServer, that is available (with the used API) since Qt 6.8.
*/
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) && __has_include(<QtHttpServer/QAbstractHttpServer>)
#define IMTREST_HTTP_SERVER_AVAILABLE
#endif

#ifdef IMTREST_HTTP_SERVER_AVAILABLE


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QSslConfiguration>

// ImtCore includes
#include <imtrest/IHttpTransportHandler.h>


namespace imtrest
{


class CHttpServerShard;
class CHttpTransport;


/**
	Listening socket of the HTTP transport.
	Accepts native socket descriptors in the thread of the transport and forwards them to the I/O threads.
*/
class CHttpListener: public QTcpServer
{
public:
	explicit CHttpListener(CHttpTransport& transport);

protected:
	// reimplemented (QTcpServer)
	virtual void incomingConnection(qintptr socketDescriptor) override;

private:
	CHttpTransport& m_transport;
};


/**
	HTTP(S)/WebSocket(S) network transport based on QtHttpServer.

	One listening socket accepts the connections and distributes them over a fixed pool of I/O threads.
	Each I/O thread runs its own QAbstractHttpServer instance (CHttpServerShard), which does
	the HTTP/1.1 parsing, keep-alive handling and response writing for the connections assigned to this thread.
	Requests are delivered to IHttpTransportHandler, responses come back asynchronously through \c SendResponse.
	WebSocket upgrade requests are detected before the HTTP layer and handed over to the handler as raw sockets,
	so the WebSocket server can do the handshake with its own settings (subprotocols etc.).
*/
class CHttpTransport: public QObject
{
public:
	struct Configuration
	{
		/**
			Number of I/O threads. 0 means QThread::idealThreadCount().
		*/
		int ioThreadCount = 0;

		/**
			Timeout (in ms) for the TLS handshake and the headers of the first request on a new connection.
			0 disables the timeout.
		*/
		int requestHeaderTimeout = 10000;

		/**
			Timeout (in ms) for idle keep-alive connections. 0 disables the timeout.
		*/
		int keepAliveTimeout = 0;

		/**
			Size of the listen backlog of the server socket.
		*/
		int listenBacklogSize = 1024;

		/**
			If enabled, the WebSocket upgrade requests are forwarded to IHttpTransportHandler::OnWebSocketHandshake.
		*/
		bool isWebSocketEnabled = false;
	};

	explicit CHttpTransport(IHttpTransportHandler& handler, QObject* parentPtr = nullptr);
	~CHttpTransport() override;

	/**
		Set the configuration. Must be called before the first \c Listen.
	*/
	void SetConfiguration(const Configuration& configuration);
	const Configuration& GetConfiguration() const;

	/**
		Start listening on the given address and port.
		If \c sslConfigurationPtr is not \c nullptr, all connections are encrypted using this configuration.
		Already existing connections are not affected by a new call of \c Listen.
	*/
	bool Listen(const QHostAddress& address, quint16 port, const QSslConfiguration* sslConfigurationPtr = nullptr);

	/**
		Stop listening. Existing connections are not affected.
	*/
	void StopListening();
	bool IsListening() const;
	quint16 GetServerPort() const;
	QString GetErrorString() const;

	/**
		Stop listening, close all connections and stop the I/O threads.
	*/
	void Shutdown();

	/**
		Send a response for the request with the given ID.
		The method is thread-safe and never blocks on the network I/O.
		\return \c false if the request is unknown (already answered or the client has disconnected).
	*/
	bool SendResponse(const QByteArray& requestId, const IHttpTransportHandler::ResponseData& response) const;

	int GetIoThreadCount() const;
	int GetActiveConnectionCount() const;

protected:
	// Called by CHttpListener in the thread of the transport.
	void DispatchSocketDescriptor(qintptr socketDescriptor);

	// Called by CHttpServerShard in its I/O thread.
	void RegisterRequest(const QByteArray& requestId, CHttpServerShard* shardPtr);
	void UnregisterRequest(const QByteArray& requestId);
	void TransferWebSocket(QTcpSocket* socketPtr);

private:
	bool EnsureIoThreadsStarted();

private:
	friend class CHttpListener;
	friend class CHttpServerShard;

	struct IoThread
	{
		QThread* threadPtr = nullptr;
		QObject* contextPtr = nullptr;
		CHttpServerShard* shardPtr = nullptr;
	};

	IHttpTransportHandler& m_handler;
	Configuration m_configuration;

	CHttpListener* m_listenerPtr;
	bool m_isSecure;
	QSslConfiguration m_sslConfiguration;

	QList<IoThread> m_ioThreads;
	int m_nextIoThreadIndex;

	mutable QMutex m_requestRoutesMutex;
	mutable QHash<QByteArray, CHttpServerShard*> m_requestRoutes;
};


} // namespace imtrest


#endif // IMTREST_HTTP_SERVER_AVAILABLE


