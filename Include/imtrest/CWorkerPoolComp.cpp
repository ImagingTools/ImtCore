// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWorkerPoolComp.h>

// Qt includes
#include <QtCore/QCoreApplication>


namespace imtrest
{


CWorkerPoolComp::CWorkerPoolComp()
	:m_pool(*this)
{
	connect(
				QCoreApplication::instance(),
				&QCoreApplication::aboutToQuit,
				this,
				&CWorkerPoolComp::AboutToQuit,
				Qt::DirectConnection);
}


// reimplemented (imtrest::IWorkerTaskQueue)

bool CWorkerPoolComp::PostTask(const QByteArray& orderingKey, Task task)
{
	return m_pool.PostTask(orderingKey, std::move(task));
}


istd::IPolymorphic* CWorkerPoolComp::GetWorkerContext() const
{
	return m_pool.GetWorkerContext();
}


void CWorkerPoolComp::SetWorkerContextFactory(WorkerContextFactory workerContextFactory)
{
	m_pool.SetWorkerContextFactory(std::move(workerContextFactory));
}


// reimplemented (icomp::CComponentBase)

void CWorkerPoolComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_pool.SetThreadsLimit(*m_threadsLimitAttrPtr);
}


void CWorkerPoolComp::AboutToQuit()
{
	m_pool.Shutdown();
}


} // namespace imtrest
