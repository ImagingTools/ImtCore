// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QSet>

// ImtCore includes
#include <imtrest/IWorkerTaskQueue.h>


namespace imtrest
{


/**
	Ordering policy of the worker pool: which pending task may be started now.

	Holds the pending tasks, the ordering keys currently in flight and the shutting-down flag,
	and answers the one question the pool asks of it - "is there a task I may hand to an idle
	worker, and which one". It knows nothing about threads, about what a task does, or about
	the component framework, so the guarantees \ref IWorkerTaskQueue makes about ordering can
	be exercised on their own.

	Not thread-safe: the pool owning it serialises every call on its own mutex.
*/
class CWorkerTaskSchedule
{
public:
	typedef IWorkerTaskQueue::Task Task;

	struct Item
	{
		Task task;
		QByteArray orderingKey;
	};

	/**
		Append \a task to the end of the queue.

		\return false, leaving the queue unchanged, once \ref Shutdown has been called.
	*/
	bool Append(const QByteArray& orderingKey, Task task);

	/**
		Remove and return the first task that may run now, marking its key as in flight.

		The scan runs from the front, so tasks sharing a key leave the queue in the order they
		were appended; a task whose key is in flight is skipped rather than stopping the scan,
		so one busy key cannot hold up unrelated work.

		\return false if nothing is currently dispatchable, leaving \a result untouched.
	*/
	bool TakeDispatchable(Item& result);

	/**
		Whether \ref TakeDispatchable would return a task now.
	*/
	bool HasDispatchable() const;

	/**
		Report that the task holding \a orderingKey has finished, making the next task that
		carries it dispatchable again. A no-op for an empty key.
	*/
	void ReleaseKey(const QByteArray& orderingKey);

	/**
		Refuse all further tasks and hand back the pending ones, which the caller destroys -
		queued tasks are dropped by a shutdown and never run.
	*/
	QList<Item> Shutdown();

	bool IsShuttingDown() const;
	int GetPendingCount() const;
	bool IsKeyInFlight(const QByteArray& orderingKey) const;

private:
	/**
		Index of the first pending task that may run now, or -1 if there is none.
	*/
	int FindDispatchableIndex() const;

private:
	QList<Item> m_pendingList;
	QSet<QByteArray> m_inFlightKeys;
	bool m_isShuttingDown = false;
};


} // namespace imtrest
