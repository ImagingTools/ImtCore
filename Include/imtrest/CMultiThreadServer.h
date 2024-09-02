#pragma once


// Qt includes
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QTimer>
#include <QtCore/QPointer>
#include <QtCore/QReadWriteLock>
#include <QtNetwork/QTcpSocket>

#if QT_VERSION >= 0x060400
#include <QtNetwork/QSslServer>
#endif
#include <QtNetwork/QSslConfiguration>

// ImtCore  includes
#include <imtrest/CTcpServerComp.h>
#include <imtrest/CHttpRequest.h>
#include <imtrest/IRequestManager.h>


namespace imtrest
{


class CMultiThreadServer;
class CSocketThread;


class CSocket: public QObject
{
	Q_OBJECT
public:
	CSocket(CSocketThread* rootSocket, imtrest::IRequest* request, bool secureConnection, const QSslConfiguration& sslConfiguration, qintptr socketDescriptor);
	~CSocket();

public Q_SLOTS:
	void HandleReadyRead();
	void Disconnected();
	void OnSendResponse(ConstResponsePtr response);
	void Abort();
	void TimeOut();

private:
	QTimer m_startTimer;
	CSocketThread* m_rootSocket;
	QPointer<QTcpSocket> m_socket;
	istd::TDelPtr<IRequest> m_requestPtr;
};


class CSocketThread:
			public QThread,
			virtual public IRequestServlet,
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
	imtrest::IRequestServlet* GetRequestServlet();

	[[nodiscard]] bool IsSecureConnection() const;
	void EnableSecureConnection(bool isSecureConnection = true);

	// reimplemented (QThread)
	void run() override;

	// reimplemented (IRequestHandler)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual ConstResponsePtr ProcessRequest(const IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

	// reimplemented (ISender)
	virtual bool SendResponse(ConstResponsePtr& response) const override;
	virtual bool SendRequest(ConstRequestPtr& reguest) const override;

	virtual IRequest* CreateRequest();

Q_SIGNALS:
	void Error(QTcpSocket::SocketError socketerror);
	void SocketDisconnected(QByteArray requestId);
	void OnSendResponse(ConstResponsePtr response) const;
	void Abort();

private:
	CMultiThreadServer* m_server;
	qintptr m_socketDescriptor;
	imtrest::IProtocolEngine* m_enginePtr;
	imtrest::IRequestServlet* m_requestHandlerPtr;
	mutable QMutex m_socketDescriptorMutex;
	mutable QMutex m_statusMutex;
	Status m_status;
	istd::TDelPtr<CSocket> m_socket;
	bool m_isSecureConnection;
	const QSslConfiguration& m_sslConfiguration;

	QByteArray m_requestId;
};


class CMultiThreadServer :
#if QT_VERSION >= 0x060400
		public QSslServer,
#else
		public QTcpServer,
#endif
			virtual public ilog::CLoggerBase,
			virtual public IRequestManager
{
	Q_OBJECT
public:
#if QT_VERSION >= 0x060400
	typedef QSslServer BaseClass;
#else
	typedef QTcpServer BaseClass;
#endif
	typedef ilog::CLoggerBase BaseClass2;

	explicit CMultiThreadServer(CTcpServerComp* rootServer);
	virtual ~CMultiThreadServer();

	imtrest::IRequestServlet* GetRequestServlet();
	imtrest::IProtocolEngine* GetProtocolEngine();

	[[nodiscard]] bool IsSecureConnection() const;
	void EnableSecureConnection(bool isSecureConnection = true);

	/**
		\brief Sets the sslConfiguration to use for all following incoming connections.
		\warning This method is only for secure connection. Call \c EnableSecureConnection before
		\warning You MUST call this method BEFORE start server!
		\sa QSslServer::setSslConfiguration
		\link https://doc.qt.io/qt-6/qsslserver.html#setSslConfiguration
	 */
	void SetSslConfiguration(const QSslConfiguration& sslConfiguration);

	// reimplemented (imtrest::IRequestManager)
	virtual const ISender* GetSender(const QByteArray& requestId) const override;

Q_SIGNALS:
	void NewThreadConnection(const IRequest* request, const QByteArray& subCommandId);

public Q_SLOTS:
	void Disconnected(QByteArray requestId);

protected Q_SLOTS:
	void ShutdownServer();

protected:
	void AddSocketDescriptor(qintptr socketDescriptor);
	qintptr GetFirstSocketDescriptor();

	// reimplemented (QTcpServer)
	void incomingConnection(qintptr socketDescriptor) override;

protected:
	QList<QPointer<CSocketThread>> m_threadSocketList;
	CTcpServerComp& m_rootServer;
	mutable QList<qintptr> m_descriptorList;
	mutable QMutex m_descriptorListMutex;
	mutable QReadWriteLock m_threadSocketListGuard;
	bool m_isActive;
	bool m_isSecureConnection;
	QSslConfiguration m_sslConfiguration;
};


} // namespace imtrest


