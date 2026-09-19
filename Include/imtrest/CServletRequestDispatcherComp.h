// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/CServletRequestDispatcher.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IResponseDispatcher.h>
#include <imtrest/IWorkerTaskQueue.h>


namespace imtrest
{


/**
	The transport path of a worker pool, as a component: requests in front, tasks behind.

	Each request becomes a task on the referenced pool. The servlet running it is per worker,
	so this component is also the one that gives the pool its worker context factory -
	RequestHandler - because it is the client that needs a context at all.
*/
class CServletRequestDispatcherComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			public imtrest::IRequestServlet
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CServletRequestDispatcherComp);
		I_REGISTER_INTERFACE(IRequestServlet)
		I_ASSIGN(m_taskQueueCompPtr, "WorkerTaskQueue", "Worker pool the requests are run on", true, "WorkerTaskQueue");
		I_ASSIGN(m_requestHandlerCompPtr, "RequestHandler", "Servlet factory, one instance per pool worker", true, "RequestHandler");
		I_ASSIGN(m_requestManagerCompPtr, "RequestManager", "Receiver of the responses", true, "RequestManager");
	I_END_COMPONENT

	CServletRequestDispatcherComp();

	// reimplemented (imtrest::IRequestServlet)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual ConstResponsePtr ProcessRequest(const IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_REF(imtrest::IWorkerTaskQueue, m_taskQueueCompPtr);
	I_FACT(imtrest::IRequestServlet, m_requestHandlerCompPtr);
	I_REF(imtrest::IResponseDispatcher, m_requestManagerCompPtr);

	mutable CServletRequestDispatcher m_requestDispatcher;
};


} // namespace imtrest
