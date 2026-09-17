// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtrest/IRequestServlet.h>
#include <imtrest/IWorkerTaskQueue.h>

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QMutex>


namespace imtrest
{

class CWorkerManagerComp;
class CWorkerThread;


class CWorker: public QObject
{
	Q_OBJECT
public:
	typedef IWorkerTaskQueue::Task Task;

	CWorker(imtrest::IRequestServletPtr&& requestServletPtr, CWorkerThread* workerThread);

public Q_SLOTS:
	void ProcessRequest(const IRequest* request, const QByteArray& subCommandId);

public:
	/**
		Run \a task on this worker's thread, then report back so that \a orderingKey is
		released and the next queued work item is dispatched. The servlet is not involved
		and no response is sent - this is the IWorkerTaskQueue arm of the pool.

		Deliberately a plain method rather than a slot: it is always reached through a
		captured-argument lambda (CWorkerThread::PostTask), so Task never has to be a
		registered queued metatype.
	*/
	void ProcessTask(Task task, const QByteArray& orderingKey);

private:
	IRequestServletPtr m_requestServletPtr;
	mutable QMutex m_processMutex;

	CWorkerThread* m_workerThread;
};


} // namespace imtrest


