// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CSubscriptionCheckSchedule.h>


// STL includes
#include <algorithm>


namespace imtservergql
{


static const qint64 s_staleRetryMs = 1000;
static const qint64 s_unavailableRetryMs = 10000;


// public methods

CSubscriptionCheckSchedule::CSubscriptionCheckSchedule(qint64 maxCheckIntervalMs, qint64 gracePeriodMs)
	:m_maxCheckIntervalMs(maxCheckIntervalMs),
	m_gracePeriodMs(gracePeriodMs)
{
}


void CSubscriptionCheckSchedule::Add(const QByteArray& key, qint64 nowMs, qint64 deadlineMs)
{
	if (m_entries.contains(key)){
		return;
	}

	Entry entry;
	entry.dueMs = GetDueTime(nowMs, deadlineMs);
	m_entries.insert(key, entry);
}


void CSubscriptionCheckSchedule::Remove(const QByteArray& key)
{
	m_entries.remove(key);
}


bool CSubscriptionCheckSchedule::Contains(const QByteArray& key) const
{
	return m_entries.contains(key);
}


void CSubscriptionCheckSchedule::OnAccessChanged(qint64 nowMs, const QByteArray& key)
{
	auto it = key.isEmpty() ? m_entries.begin() : m_entries.find(key);
	for (; it != m_entries.end(); ++it){
		Entry& entry = *it;
		if (entry.isChecking){
			entry.isChangePending = true;
		}
		else{
			// A repetition keeps its pace: a change does not make a failing source asked more often
			const qint64 earliestMs = (entry.graceStartMs >= 0) ? std::max(nowMs, entry.attemptStartMs + entry.retryMs) : nowMs;
			entry.dueMs = std::min(entry.dueMs, earliestMs);
		}
		if (!key.isEmpty()){
			break;
		}
	}
}


QByteArrayList CSubscriptionCheckSchedule::TakeDue(qint64 nowMs)
{
	QByteArrayList retVal;
	for (auto it = m_entries.begin(); it != m_entries.end(); ++it){
		if (!it->isChecking && it->dueMs <= nowMs){
			it->isChecking = true;
			it->attemptStartMs = nowMs;
			it->attemptId = ++m_lastAttemptId;
			retVal << it.key();
		}
	}

	return retVal;
}


qint64 CSubscriptionCheckSchedule::GetAttemptId(const QByteArray& key) const
{
	const auto it = m_entries.constFind(key);

	return (it != m_entries.cend() && it->isChecking) ? it->attemptId : -1;
}


bool CSubscriptionCheckSchedule::IsCurrentAttempt(const QByteArray& key, qint64 attemptId) const
{
	return attemptId >= 0 && GetAttemptId(key) == attemptId;
}


void CSubscriptionCheckSchedule::OnChecked(const QByteArray& key, CheckResult result, qint64 nowMs, qint64 deadlineMs)
{
	auto it = m_entries.find(key);
	if (it == m_entries.end()){
		return;
	}

	Entry& entry = *it;
	const qint64 unconfirmedSinceMs = GetUnconfirmedSince(entry);
	if (unconfirmedSinceMs >= 0 && nowMs >= unconfirmedSinceMs + m_gracePeriodMs){
		return;
	}

	entry.isChecking = false;

	if (result == CR_CONFIRMED){
		entry.graceStartMs = -1;
		entry.retryMs = 0;
		entry.dueMs = entry.isChangePending ? nowMs : GetDueTime(nowMs, deadlineMs);
	}
	else{
		// The grace period starts once, with the first attempt that did not confirm; only a confirmation ends it
		if (entry.graceStartMs < 0){
			entry.graceStartMs = entry.attemptStartMs;
		}
		entry.retryMs = (result == CR_STALE) ? s_staleRetryMs : s_unavailableRetryMs;
		entry.dueMs = std::max(nowMs, entry.attemptStartMs + entry.retryMs);
	}
	entry.isChangePending = false;
}


QByteArrayList CSubscriptionCheckSchedule::TakeExpired(qint64 nowMs)
{
	QByteArrayList retVal;
	for (auto it = m_entries.begin(); it != m_entries.end(); ){
		const qint64 unconfirmedSinceMs = GetUnconfirmedSince(*it);
		if (unconfirmedSinceMs >= 0 && nowMs >= unconfirmedSinceMs + m_gracePeriodMs){
			retVal << it.key();
			it = m_entries.erase(it);
		}
		else{
			++it;
		}
	}

	return retVal;
}


qint64 CSubscriptionCheckSchedule::GetNextWakeTime() const
{
	qint64 retVal = -1;
	for (const Entry& entry : m_entries){
		qint64 wakeMs = entry.isChecking ? -1 : entry.dueMs;
		const qint64 unconfirmedSinceMs = GetUnconfirmedSince(entry);
		if (unconfirmedSinceMs >= 0){
			const qint64 graceEndMs = unconfirmedSinceMs + m_gracePeriodMs;
			wakeMs = (wakeMs < 0) ? graceEndMs : std::min(wakeMs, graceEndMs);
		}
		if (wakeMs >= 0 && (retVal < 0 || wakeMs < retVal)){
			retVal = wakeMs;
		}
	}

	return retVal;
}


// private methods

qint64 CSubscriptionCheckSchedule::GetDueTime(qint64 nowMs, qint64 deadlineMs) const
{
	const qint64 controlMs = nowMs + m_maxCheckIntervalMs;
	if (deadlineMs < 0){
		return controlMs;
	}

	// A deadline already passed is not checked in a loop: the next check may find it moved, as a refreshed session does
	return std::min(controlMs, deadlineMs <= nowMs ? nowMs + s_unavailableRetryMs : deadlineMs);
}


qint64 CSubscriptionCheckSchedule::GetUnconfirmedSince(const Entry& entry)
{
	if (entry.graceStartMs >= 0){
		return entry.graceStartMs;
	}

	return entry.isChecking ? entry.attemptStartMs : -1;
}


} // namespace imtservergql
