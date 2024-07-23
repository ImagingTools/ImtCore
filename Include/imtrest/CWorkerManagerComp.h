#pragma once


// Qt includes
#include <QtCore/QThread>
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IRequestManager.h>


namespace imtrest
{

class CWorkerManagerComp;
class CWorkerThread;


class CWorker: public QObject
{
	Q_OBJECT
public:
	CWorker(const imtrest::IRequestServlet* requestServletPtr, CWorkerThread* workerThread);

Q_SIGNALS:
	void FinishProcess(const IRequest* request, const QByteArray& subCommandId);

public Q_SLOTS:
	void ProcessRequest(const IRequest* request, const QByteArray& subCommandId);

private:
	const imtrest::IRequestServlet* m_requestServletPtr;

	CWorkerThread* m_workerThread;
};


class CWorkerThread: public QThread
{
	Q_OBJECT
public:
	CWorkerThread(const CWorkerManagerComp* workerManager, const QByteArray& subCommandId);

	enum Status
	{
		ST_PROCESS,
		ST_CLOSE
	};

	Status GetStatus();
	void SetStatus(Status status);
	void SetRequestPtr(const IRequest* requestPtr);
	const ISender* GetSender(const QByteArray& requestId);

	//reimplemented (QThread)
	void run() override;

Q_SIGNALS:
	void StartProcess(const IRequest* request, const QByteArray& subCommandId);
	void FinishProcess(const IRequest* request, const QByteArray& subCommandId);

protected Q_SLOTS:
	void OnStarted();
	void OnFinishProcess(const IRequest* request, const QByteArray& subCommandId);

private:
	imtrest::IRequestServlet* m_requestServletPtr;
	Status m_status;
	mutable CWorkerManagerComp* m_workerManager;
	istd::TDelPtr<CWorker> m_workerPtr;
	const IRequest* m_requestPtr;
	mutable QMutex m_statusMutex;
	QByteArray m_subCommandId;
};


class CWorkerManagerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			public imtrest::IRequestServlet
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CWorkerManagerComp);
		I_REGISTER_INTERFACE(IRequestServlet)
		I_ASSIGN(m_requestHandlerCompPtr, "RequestHandler", "Request handler registered for the server", true, "RequestHandler");
		I_ASSIGN(m_requestManagerCompPtr, "RequestManager", "Request manager registered for the server", true, "RequestManager");
		I_ASSIGN(m_threadsLimitAttrPtr, "ThreadsLimit", "Limit of threads", true, 5);
	I_END_COMPONENT

	CWorkerManagerComp();

	IRequestServlet* CreateServlet();
	const ISender* GetSender(const QByteArray& requestId);

	// reimplemented (IRequestHandler)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual ConstResponsePtr ProcessRequest(const IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

Q_SIGNALS:
	void Start(const IRequest* request);

protected Q_SLOTS:
	void OnFinish(const IRequest* request, const QByteArray& subCommandId);
	void AboutToQuit();

private:
	I_FACT(imtrest::IRequestServlet, m_requestHandlerCompPtr);
	I_REF(imtrest::IRequestManager, m_requestManagerCompPtr);
	I_ATTR(int, m_threadsLimitAttrPtr);

	mutable QList<CWorkerThread*> m_workerList;
	mutable QList<const IRequest*> m_requestList;
	mutable QMutex m_requestListMutex;
};


} // namespace imtrest


