#pragma once


// Qt includes
#include <QtCore/QReadWriteLock>

#if QT_VERSION >= 0x060400
#include <QtNetwork/QSslServer>
#endif
#include <QtNetwork/QSslConfiguration>

// ImtCore  includes
#include <imthttp/CTcpServerComp.h>
#include <imthttp/IRequestManager.h>
#include <imthttp/CSocketThread.h>


namespace imtrest
{
class IRequestServlet;
}


namespace imthttp
{


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

	imthttp::imtrest::IRequestServlet* GetRequestServlet();
	imthttp::IProtocolEngine* GetProtocolEngine();

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

	// reimplemented (imthttp::IRequestManager)
	virtual const ISender* GetSender(const QByteArray& requestId) const override;

Q_SIGNALS:
	void NewThreadConnection(const IRequest* request, const QByteArray& subCommandId);

public Q_SLOTS:
	void Disconnected(QByteArray requestId);

protected Q_SLOTS:
	void ShutdownServer();

protected:
	void AddSocketDescriptor(qintptr socketDescriptor);
	qintptr PopSocketDescriptor();

	// reimplemented (QTcpServer)
	virtual void incomingConnection(qintptr socketDescriptor) override;

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


} // namespace imthttp


