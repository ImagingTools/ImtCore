#pragma once


// Qt includes
#include <QtCore/QThread>
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequestServlet.h>
#include <imtrest/IRequestManager.h>
#include <imtrest/CWorkerThread.h>


namespace imtrest
{


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


