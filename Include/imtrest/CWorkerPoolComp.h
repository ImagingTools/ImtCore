// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/CWorkerPool.h>
#include <imtrest/IWorkerTaskQueue.h>


namespace imtrest
{


/**
	A pool of worker threads, as a component.

	It is the general one: it runs tasks and has no idea what they do. The transport path is
	built on it by ServletRequestDispatcher, which is also the client supplying the per-worker
	context the requests need. A registry that only wants work run off its own thread wires
	this element alone.
*/
class CWorkerPoolComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			public imtrest::IWorkerTaskQueue
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CWorkerPoolComp);
		I_REGISTER_INTERFACE(IWorkerTaskQueue)
		I_ASSIGN(m_threadsLimitAttrPtr, "ThreadsLimit", "Limit of threads", true, 5);
	I_END_COMPONENT

	CWorkerPoolComp();

	// reimplemented (imtrest::IWorkerTaskQueue)
	virtual bool PostTask(const QByteArray& orderingKey, Task task) override;
	virtual istd::IPolymorphic* GetWorkerContext() const override;
	virtual void SetWorkerContextFactory(WorkerContextFactory workerContextFactory) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected Q_SLOTS:
	void AboutToQuit();

private:
	I_ATTR(int, m_threadsLimitAttrPtr);

	CWorkerPool m_pool;
};


} // namespace imtrest
