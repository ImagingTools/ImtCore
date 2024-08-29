#pragma once


// Qt includes
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QTimer>
#include <QtCore/QPointer>
#include <QtCore/QReadWriteLock>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QSslConfiguration>

#if QT_VERSION >= 0x060400
#include <QtNetwork/QSslServer>
#endif

// ImtCore  includes
#include <imtrest/CTcpServerComp.h>
#include <imtrest/CHttpRequest.h>
#include <imtrest/IRequestManager.h>


namespace imtrest
{


class CMultiThreadServer;
class CSocketThread;


class CSocket: public QObject,
			virtual public IRequestServlet,
			virtual public ISender
{
	Q_OBJECT
public:
	CSocket(qintptr socketDescriptor, bool secureConnection, const QSslConfiguration& sslConfiguration, CMultiThreadServer *parent);
	// CSocket(CSocketThread* rootSocket, imtrest::IRequest* request, bool secureConnection, const QSslConfiguration& sslConfiguration, qintptr socketDescriptor);
	QByteArray GetRequestId();
	imtrest::IRequestServlet* GetRequestServlet();
	[[nodiscard]] bool IsSecureConnection() const;
	void EnableSecureConnection(bool isSecureConnection = true);
	virtual IRequest* CreateRequest();

	// reimplemented (IRequestHandler)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual ConstResponsePtr ProcessRequest(const IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

	// reimplemented (ISender)
	virtual bool SendResponse(ConstResponsePtr& response) const override;
	virtual bool SendRequest(ConstRequestPtr& reguest) const override;


Q_SIGNALS:
	void Error(QTcpSocket::SocketError socketerror);
	void SocketDisconnected(QByteArray requestId);
	// void SendResponse(ConstResponsePtr response);
	void Abort();

public Q_SLOTS:
	void OnHandleReadyRead();
	void OnDisconnected();
	void OnAbort();
	void OnTimeOut();

private:
	QTimer m_startTimer;
	istd::TDelPtr<QTcpSocket> m_socket;
	istd::TDelPtr<IRequest> m_requestPtr;
	CMultiThreadServer* m_server;
	imtrest::IProtocolEngine* m_enginePtr;
	imtrest::IRequestServlet* m_requestHandlerPtr;
	mutable QMutex m_socketDescriptorMutex;
	mutable QMutex m_statusMutex;
	bool m_isSecureConnection;
	const QSslConfiguration& m_sslConfiguration;

	QByteArray m_requestId;
};


class CMultiThreadServer :
		#if QT_VERSION < 0x060400
			public QTcpServer,
		 #else
			public QSslServer,
		 #endif
			virtual public ilog::CLoggerBase,
			virtual public IRequestManager
{
	Q_OBJECT
public:
#if QT_VERSION < 0x060400
	typedef QTcpServer BaseClass;
 #else
	typedef QSslServer BaseClass;
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
	QSslConfiguration& GetSslConfiguration();

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
	QList<QPointer<CSocket>> m_socketList;
	CTcpServerComp& m_rootServer;
	mutable QList<qintptr> m_descriptorList;
	mutable QMutex m_descriptorListMutex;
	mutable QReadWriteLock m_socketListGuard;
	bool m_isActive;
	bool m_isSecureConnection;
	QSslConfiguration m_sslConfiguration;
};


} // namespace imtrest


