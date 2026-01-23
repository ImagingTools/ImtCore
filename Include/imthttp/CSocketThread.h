#pragma once


// Qt includes
#include <QtCore/QThread>
#include <QtCore/QMutex>

// ImtCore includes

#include <imthttp/ISender.h>
#include <imthttp/CSocket.h>


namespace imthttp
{

class CMultiThreadServer;

class CSocketThread:
			public QThread,
			virtual public imtrest::IRequestServlet,
			virtual public ISender
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
	imthttp::imtrest::IRequestServlet* GetRequestServlet();

	[[nodiscard]] bool IsSecureConnection() const;
	void EnableSecureConnection(bool isSecureConnection = true);

	virtual imthttp::IRequestUniquePtr CreateRequest() const;

	// reimplemented (QThread)
	virtual void run() override;

	// reimplemented (IRequestHandler)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual ConstResponsePtr ProcessRequest(const IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

	// reimplemented (ISender)
	virtual bool SendResponse(ConstResponsePtr& response) const override;
	virtual bool SendRequest(ConstRequestPtr& reguest) const override;

Q_SIGNALS:
	void Error(QTcpSocket::SocketError socketerror);
	void SocketDisconnected(QByteArray requestId);
	void OnSendResponse(ConstResponsePtr response) const;
	void Abort();

private:
	CMultiThreadServer* m_server;
	qintptr m_socketDescriptor;
	const imthttp::IProtocolEngine* m_enginePtr;
	imthttp::imtrest::IRequestServlet* m_requestHandlerPtr;
	mutable QMutex m_socketDescriptorMutex;
	mutable QMutex m_statusMutex;
	Status m_status;
	istd::TDelPtr<CSocket> m_socket;
	bool m_isSecureConnection;
	const QSslConfiguration& m_sslConfiguration;

	mutable QByteArray m_requestId;
};


} // namespace imthttp


