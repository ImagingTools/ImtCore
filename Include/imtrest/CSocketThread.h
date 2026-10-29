// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QThread>
#include <QtCore/QMutex>

// STL includes
#include <atomic>

// ImtCore includes
#include <imtrest/IRequestServlet.h>
#include <imtrest/ITransport.h>
#include <imtrest/CConnectionTimeoutTracker.h>
#include <imtrest/CSocket.h>


namespace imtrest
{

class CMultiThreadServer;

class CSocketThread:
			public QThread,
			virtual public IRequestServlet,
			virtual public ITransport
{
	Q_OBJECT
public:
	enum Status
	{
		ST_START,
		ST_PROCESS,
		ST_CLOSE
	};

	explicit CSocketThread(qintptr ID, bool secureConnection, const QSslConfiguration& sslConfiguration, CMultiThreadServer *parent);
	void SetSocketDescriptor(qintptr socketDescriptor);
	qintptr GetSocketDescriptor();
	void SetSocketStatus(Status socketStatus);
	Status GetSocketStatus();
	QByteArray GetRequestId();
	imtrest::IRequestServlet* GetRequestServlet();

	[[nodiscard]] bool IsSecureConnection() const;
	void EnableSecureConnection(bool isSecureConnection = true);

	/**
		Get the time limits of the connections.
	*/
	[[nodiscard]] CConnectionTimeoutConfig GetConnectionTimeoutConfig() const;

	/**
		Get the limit of the unsent data of a socket in bytes: a streaming response is not read while the socket has so much
		unsent data (see CHttpResponseBodyStreamer).
	*/
	[[nodiscard]] qint64 GetWriteHighWatermark() const;

	/**
		Get the type-ID of the protocol of the connection as a text (see IProtocolEngine::GetProtocolTypeId()).
		The socket classes do not interpret it.
	*/
	[[nodiscard]] QByteArray GetProtocolTypeId() const;

	/**
		Remember how the response to the request that is being processed should be sent (see IRequest::IsConnectionPersistent(),
		IRequest::IsResponseBodyOmitted() and IRequest::IsChunkedResponseAllowed()). It is called by the socket of the thread, the hints are read by the thread that
		sends the response.
	*/
	void SetResponseHints(bool isConnectionClosing, bool isBodyOmitted, bool isChunkedAllowed);

	/**
		The connection is closed after the response to the request that is being processed.
	*/
	[[nodiscard]] bool IsConnectionClosing() const;

	/**
		The response to the request that is being processed has no content.
	*/
	[[nodiscard]] bool IsResponseBodyOmitted() const;

	/**
		The response to the request that is being processed can be sent with the chunked encoding.
	*/
	[[nodiscard]] bool IsChunkedResponseAllowed() const;

	/**
		Send a response with a body that is read by parts (see IStreamingResponse). It can be called from any thread: the response
		is handed to the socket of the thread, which creates the source of the body and sends the response.
		The hints are the properties of the request that is answered, see SetResponseHints().
	*/
	bool SendStreamingResponse(const ConstResponsePtr& response, bool isConnectionClosing, bool isBodyOmitted, bool isChunkedAllowed) const;

	virtual imtrest::IRequestUniquePtr CreateRequest() const;

	// reimplemented (QThread)
	virtual void run() override;

	// reimplemented (IRequestHandler)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual ConstResponsePtr ProcessRequest(const IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

	// reimplemented (ITransport)
	virtual bool SendData(const QByteArray& data) const override;

Q_SIGNALS:
	void Error(QTcpSocket::SocketError socketerror);
	void SocketDisconnected(QByteArray requestId);
	void OnSendResponse(QByteArray data) const;
	void OnSendStreamingResponse(imtrest::ConstResponsePtr response, bool isConnectionClosing, bool isBodyOmitted, bool isChunkedAllowed) const;
	void Abort();

private:
	CMultiThreadServer* m_server;
	qintptr m_socketDescriptor;
	const imtrest::IProtocolEngine* m_enginePtr;
	imtrest::IRequestServlet* m_requestHandlerPtr;
	mutable QMutex m_socketDescriptorMutex;
	mutable QMutex m_statusMutex;
	Status m_status;
	istd::TDelPtr<CSocket> m_socket;
	bool m_isSecureConnection;
	std::atomic<bool> m_isConnectionClosing;
	std::atomic<bool> m_isResponseBodyOmitted;
	std::atomic<bool> m_isChunkedResponseAllowed;
	const QSslConfiguration& m_sslConfiguration;

	mutable QByteArray m_requestId;
};


} // namespace imtrest


