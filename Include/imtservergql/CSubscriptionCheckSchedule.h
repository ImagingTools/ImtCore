// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QHash>


namespace imtservergql
{


/**
	When the access of each set of credentials holding subscriptions is checked again.
	It has no thread and no clock: the caller gives a monotonic time in milliseconds.

	A set is checked at the latest after the control interval, earlier at its deadline (the end of its session) and after a change
	of the access. A check that does not confirm the access starts a grace period at the start of its attempt; a set whose
	access is not confirmed within it is over, a check still running included.
*/
class CSubscriptionCheckSchedule
{
public:
	enum CheckResult
	{
		CR_CONFIRMED,
		/// The access changed during the check.
		CR_STALE,
		/// Nothing is known.
		CR_UNAVAILABLE
	};

	explicit CSubscriptionCheckSchedule(qint64 maxCheckIntervalMs = 300000, qint64 gracePeriodMs = 60000);

	/**
		Credentials checked now: due at the control check, or at their deadline if earlier.
		A set already scheduled keeps its schedule.
		\param deadlineMs	-1 for none.
	*/
	void Add(const QByteArray& key, qint64 nowMs, qint64 deadlineMs = -1);
	void Remove(const QByteArray& key);
	bool Contains(const QByteArray& key) const;

	/**
		The access may have changed: every set is due, at the pace of its repetitions; a grace period goes on.
		A set being checked is checked once more after it.
		\param key	An empty key means all sets; otherwise only the named set is affected.
	*/
	void OnAccessChanged(qint64 nowMs, const QByteArray& key = QByteArray());

	/// The sets due, marked as being checked.
	QByteArrayList TakeDue(qint64 nowMs);

	/**
		The id of the attempt of a set being checked: an answer is used only while its attempt is the current one, so the late
		answer of a check made before a set was removed and added again is not taken for its own. -1 for an unknown set.
	*/
	qint64 GetAttemptId(const QByteArray& key) const;
	bool IsCurrentAttempt(const QByteArray& key, qint64 attemptId) const;

	/// The result of a check of a set taken by TakeDue; nothing for a set removed meanwhile.
	void OnChecked(const QByteArray& key, CheckResult result, qint64 nowMs, qint64 deadlineMs = -1);

	/// The sets whose access was not confirmed within the grace period, removed.
	QByteArrayList TakeExpired(qint64 nowMs);

	/// When a set is due or its grace period ends next; -1 when nothing is scheduled.
	qint64 GetNextWakeTime() const;

private:
	struct Entry
	{
		qint64 dueMs = 0;
		/// The start of the first attempt that did not confirm the access; -1 while it is confirmed.
		qint64 graceStartMs = -1;
		qint64 attemptStartMs = -1;
		qint64 retryMs = 0;
		qint64 attemptId = -1;
		bool isChecking = false;
		bool isChangePending = false;
	};

	qint64 GetDueTime(qint64 nowMs, qint64 deadlineMs) const;
	/// Since when the access is not confirmed, a running attempt included; -1 when it is.
	static qint64 GetUnconfirmedSince(const Entry& entry);

	qint64 m_maxCheckIntervalMs;
	qint64 m_gracePeriodMs;
	QHash<QByteArray, Entry> m_entries;
	qint64 m_lastAttemptId = 0;
};


} // namespace imtservergql
