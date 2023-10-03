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
	void FinishProcess(const IRequest* request);

public Q_SLOTS:
	void ProcessRequest(const IRequest* request);

private:
	const imtrest::IRequestServlet* m_requestServletPtr;

	CWorkerThread* m_workerThread;
};


class CWorkerThread: public QThread
{
	Q_OBJECT
public:
	CWorkerThread(const CWorkerManagerComp* workerManager);

	enum Status
	{
		ST_PROCESS,
		ST_CLOSE
	};

	Status GetStatus();
	void SetStatus(Status status);
	void SetRequest(const IRequest* request);
	const ISender* GetSender(const QByteArray& requestId);

	//reimplemented (QThread)
	void run() override;

Q_SIGNALS:
	void StartProcess(const IRequest* request);
	void FinishProcess(const IRequest* request);

protected Q_SLOTS:
	void OnStarted();
	void OnFinishProcess(const IRequest* request);

private:
	imtrest::IRequestServlet* m_requestServletPtr;
	Status m_status;
	mutable CWorkerManagerComp* m_workerManager;
	istd::TDelPtr<CWorker> m_workerPtr;
	const IRequest* m_request;
	mutable QMutex m_statusMutex;
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
	virtual ConstResponsePtr ProcessRequest(const IRequest& request) const override;
	virtual QByteArray GetSupportedCommandId() const override;

Q_SIGNALS:
	void Start(const IRequest* request);

protected Q_SLOTS:
	void OnFinish(const IRequest* request);
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


