// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/CServletRequestDispatcher.h>
#include <imtrest/CWorkerPool.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IResponseDispatcher.h>
#include <imtrest/IWorkerTaskQueue.h>


namespace imtrest
{


/**
	A worker pool together with the transport path that runs on it, as a single component.

	It exists for registries wiring one element for both, and is a thin shell: the pool is
	\ref CWorkerPool and the transport path is \ref CServletRequestDispatcher, which is the
	general-to-special order the two follow. A registry that wants only a pool - work that is
	not a request, and no servlet - should wire WorkerPool instead, and one that wants only the
	transport path ServletRequestDispatcher in front of it.
*/
class CWorkerManagerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			public imtrest::IRequestServlet,
			public imtrest::IWorkerTaskQueue
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CWorkerManagerComp);
		I_REGISTER_INTERFACE(IRequestServlet)
		I_REGISTER_INTERFACE(IWorkerTaskQueue)
		I_ASSIGN(m_requestHandlerCompPtr, "RequestHandler", "Request handler registered for the server, used as the worker context", false, "RequestHandler");
		I_ASSIGN(m_requestManagerCompPtr, "RequestManager", "Request manager registered for the server", false, "RequestManager");
		I_ASSIGN(m_threadsLimitAttrPtr, "ThreadsLimit", "Limit of threads", true, 5);
	I_END_COMPONENT

	CWorkerManagerComp();

	// reimplemented (imtrest::IRequestServlet)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual ConstResponsePtr ProcessRequest(const IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

	// reimplemented (imtrest::IWorkerTaskQueue)
	virtual bool PostTask(const QByteArray& orderingKey, Task task) override;
	virtual istd::IPolymorphic* GetWorkerContext() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected Q_SLOTS:
	void AboutToQuit();

private:
	I_FACT(imtrest::IRequestServlet, m_requestHandlerCompPtr);
	I_REF(imtrest::IResponseDispatcher, m_requestManagerCompPtr);
	I_ATTR(int, m_threadsLimitAttrPtr);

	// Declared before the pool so that it outlives it: the pool's destructor joins the
	// workers, and a task still running holds a request owned by the dispatcher.
	mutable CServletRequestDispatcher m_requestDispatcher;
	mutable CWorkerPool m_pool;
};


} // namespace imtrest
