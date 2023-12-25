#pragma once

// Qt includes
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QTimer>

// ImtCore  includes
#include <imtrest/CTcpServerComp.h>
#include <imtrest/CHttpRequest.h>
#include <imtrest/IRequestManager.h>



namespace imtrest {

class CMultiThreadServer;
class CSocketThread;

class CSocket : public QObject
{
	Q_OBJECT
public:
	CSocket(CSocketThread* rootSocket, imtrest::IRequest* request, qintptr socketDescriptor);

public Q_SLOTS:
	void HandleReadyRead();
	void Disconnected();
	void OnSendResponse(ConstResponsePtr response);
	void Abort();
	void TimeOut();

private:
	QTimer m_startTimer;
	CSocketThread* m_rootSocket;
	istd::TDelPtr<QTcpSocket> m_socket;
	istd::TDelPtr<IRequest> m_requestPtr;
};


class CSocketThread : public QThread, virtual public IRequestServlet, virtual public ISender
{
	Q_OBJECT
  public:
	enum Status
	{
		ST_START,
		ST_PROCESS,
		ST_CLOSE
	};

	explicit CSocketThread(qintptr ID, CMultiThreadServer *parent);
	void SetSocketDescriptor(qintptr socketDescriptor);
	qintptr GetSocketDescriptor();
	void SetSocketStatus(Status socketStatus);
	Status GetSocketStatus();
	QByteArray GetRequestId();
	imtrest::IRequestServlet* GetRequestServlet();


	// reimplemented (QThread)
	void run() override;

	// reimplemented (IRequestHandler)
	virtual ConstResponsePtr ProcessRequest(const IRequest& request) const override;
	virtual QByteArray GetSupportedCommandId() const override;

	// reimplemented (ISender)
	virtual bool SendResponse(ConstResponsePtr& response) const override;
	virtual bool SendRequest(ConstRequestPtr& reguest) const override;

Q_SIGNALS:
	void Error(QTcpSocket::SocketError socketerror);
	void SocketDisconnected(QByteArray requestId);
//	void ParcerFinished(const IRequest* request) const;
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

	QByteArray m_requestId;
};



class CMultiThreadServer : public QTcpServer, virtual public IRequestManager
{
	Q_OBJECT
public:
	explicit CMultiThreadServer(CTcpServerComp* rootServer);
    imtrest::IRequestServlet* GetRequestServlet();
    imtrest::IProtocolEngine* GetProtocolEngine();


	// reimplemented (imtrest::IRequestManager)
	virtual const IRequest* GetRequest(const QByteArray& requestId) const override;
	virtual const ISender* GetSender(const QByteArray& requestId) const override;

Q_SIGNALS:
	void NewThreadConnection(const IRequest* request);

public Q_SLOTS:
	void Disconnected(QByteArray requestId);
	void AboutToQuit();

protected:
	void AddSocketDescriptor(qintptr socketDescriptor);
	qintptr GetFirstSocketDescriptor();

	// reimplemented (QTcpServer)
	void incomingConnection(qintptr socketDescriptor) override;

	QList<CSocketThread*> m_threadSocketList;
	CTcpServerComp& m_rootServer;
	mutable QList<qintptr> m_descriptorList;
	mutable QMutex m_descriptorListMutex;
	bool m_isActive;
};


} // namespace imtrest
