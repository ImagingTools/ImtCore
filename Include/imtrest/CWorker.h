// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QObject>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtrest/IWorkerTaskQueue.h>


namespace imtrest
{

class CWorkerThread;


/**
	The object a pool worker runs its tasks on, living on the worker thread.

	It knows nothing about what a task does. Anything a task needs from the thread it runs on
	is reached through the worker context, which the pool created for this thread and which
	\ref IWorkerTaskQueue::GetWorkerContext hands back for the duration of the call.
*/
class CWorker: public QObject
{
	Q_OBJECT
public:
	typedef IWorkerTaskQueue::Task Task;

	CWorker(CWorkerThread* workerThread, istd::IPolymorphic* contextPtr);

	/**
		Run \a task on this worker's thread, then report back so that \a orderingKey is
		released and the next queued task is dispatched.

		Deliberately a plain method rather than a slot: it is always reached through a
		captured-argument lambda (CWorkerThread::PostTask), so Task never has to be a
		registered queued metatype.
	*/
	void ProcessTask(Task task, const QByteArray& orderingKey);

private:
	mutable QMutex m_processMutex;

	CWorkerThread* m_workerThread;
	istd::IPolymorphic* m_contextPtr;
};


} // namespace imtrest
