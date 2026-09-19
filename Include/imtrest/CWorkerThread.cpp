// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWorkerThread.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QMetaObject>

// ImtCore includes
#include <imtrest/CWorkerManagerComp.h>


namespace imtrest
{


CWorkerThread::CWorkerThread(const CWorkerManagerComp* workerManager, const QByteArray& subCommandId)
	: m_status(ST_PROCESS),
	  m_workerManager(nullptr),
	  m_workerPtr(nullptr),
	  m_requestPtr(nullptr),
	  m_subCommandId(subCommandId)
{
	m_workerManager = const_cast<CWorkerManagerComp*>(workerManager);
}


void CWorkerThread::SetServlet(IRequestServletPtr&& servletPtr)
{
	m_servletPtr = std::move(servletPtr);
}


CWorkerThread::Status CWorkerThread::GetStatus()
{
	QMutexLocker lock(&m_statusMutex);

	return m_status;
}


void CWorkerThread::SetStatus(Status status)
{
	QMutexLocker lock(&m_statusMutex);

	m_status = status;
}


void CWorkerThread::SetRequestPtr(const IRequest* requestPtr)
{
	m_requestPtr = requestPtr;
}


void CWorkerThread::SetPendingTask(Task task, const QByteArray& orderingKey)
{
	m_pendingTask = std::move(task);
	m_pendingOrderingKey = orderingKey;
}


bool CWorkerThread::SendResponse(const QByteArray& requestId, ConstResponsePtr& response)
{
	if (m_workerManager == nullptr){
		return false;
	}

	return m_workerManager->SendResponse(requestId, response);
}


//reimplemented (QThread)

void CWorkerThread::run()
{
	// Servlet must already be created on the manager thread (SetServlet). Creating it
	// here ran factory OnComponentCreated() on the worker and could parent QObjects to
	// qApp → one-shot "Cannot create children... Parent is QCoreApplication".

	if (!m_servletPtr.IsValid()){
		// Fallback only if a caller forgot SetServlet (should not happen).
		if (m_workerManager != nullptr){
			m_servletPtr = m_workerManager->CreateServlet();
		}
	}

	if (!m_servletPtr.IsValid()){
		Q_ASSERT(false);

		return;
	}

	// CWorker is a QObject bound to THIS worker thread. ProcessRequest is posted so it
	// runs only after exec() pumps the event loop (timers / nested loops need a dispatcher).
	m_workerPtr.SetPtr(new CWorker(std::move(m_servletPtr), this));
	m_workerPtr->moveToThread(this);

	// Exactly one of the two arms is set by the manager before start(): a thread is
	// created to run one queued work item, which is either a request or a task.
	if (m_requestPtr != nullptr){
		PostRequest(m_requestPtr, m_subCommandId);
	}
	else if (m_pendingTask){
		PostTask(std::move(m_pendingTask), m_pendingOrderingKey);

		m_pendingTask = Task();
	}

	exec();
}


void CWorkerThread::PostRequest(const IRequest* requestPtr, const QByteArray& subCommandId)
{
	CWorker* workerPtr = m_workerPtr.GetPtr();
	if (workerPtr == nullptr){
		return;
	}

	// Deliver to the worker on its own thread via a captured-argument lambda. This is
	// the fix for the hang: the previous queued StartProcess signal carried a raw
	// 'const IRequest*', which is not a registered queued metatype, so Qt dropped the
	// call and ProcessRequest never ran (worker stuck idle in exec()). A lambda captures
	// the pointer directly and needs no metatype.
	QMetaObject::invokeMethod(
				workerPtr,
				[workerPtr, requestPtr, subCommandId]() {
					workerPtr->ProcessRequest(requestPtr, subCommandId);
				},
				Qt::QueuedConnection);
}


void CWorkerThread::PostTask(Task task, const QByteArray& orderingKey)
{
	CWorker* workerPtr = m_workerPtr.GetPtr();
	if (workerPtr == nullptr){
		return;
	}

	// Same captured-argument lambda as PostRequest: a queued signal would need every
	// argument type registered as a queued metatype, and Qt silently drops the call
	// otherwise, leaving the worker idle in exec() forever.
	QMetaObject::invokeMethod(
				workerPtr,
				[workerPtr, task = std::move(task), orderingKey]() mutable {
					workerPtr->ProcessTask(std::move(task), orderingKey);
				},
				Qt::QueuedConnection);
}


void CWorkerThread::NotifyFinished(const IRequest* requestPtr, const QByteArray& subCommandId)
{
	CWorkerManagerComp* workerManager = m_workerManager;
	if (workerManager == nullptr){
		return;
	}

	// Hop to the manager's own thread (same lambda rationale as PostRequest); the
	// manager's OnFinish frees the request and dispatches the next queued one.
	QMetaObject::invokeMethod(
				workerManager,
				[workerManager, requestPtr, subCommandId]() {
					workerManager->OnFinish(requestPtr, subCommandId);
				},
				Qt::QueuedConnection);
}



void CWorkerThread::NotifyTaskFinished(const QByteArray& orderingKey)
{
	CWorkerManagerComp* workerManager = m_workerManager;
	if (workerManager == nullptr){
		return;
	}

	// Hop to the manager's own thread (same lambda rationale as PostRequest); the
	// manager's OnTaskFinish releases the ordering key and dispatches the next queued
	// work item.
	QMetaObject::invokeMethod(
				workerManager,
				[workerManager, orderingKey]() {
					workerManager->OnTaskFinish(orderingKey);
				},
				Qt::QueuedConnection);
}


} // namespace imtrest
