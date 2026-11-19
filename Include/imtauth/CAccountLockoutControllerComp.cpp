// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CAccountLockoutControllerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QMutexLocker>


namespace imtauth
{


// reimplemented (imtauth::IAccountLockoutController)

bool CAccountLockoutControllerComp::IsAccountLocked(const QByteArray& login, qint64* remainingLockoutSecsPtr) const
{
	if (*m_maxFailedAttemptsAttrPtr <= 0){
		return false;
	}

	QMutexLocker locker(&m_mutex);

	auto iter = m_accountStates.find(login);
	if (iter == m_accountStates.end()){
		return false;
	}

	qint64 nowSecs = QDateTime::currentSecsSinceEpoch();
	if (!IsLockedInternal(*iter, nowSecs, remainingLockoutSecsPtr)){
		if (!iter->isLocked && iter->failedAttemptTimes.isEmpty()){
			m_accountStates.erase(iter);
		}

		return false;
	}

	return true;
}


int CAccountLockoutControllerComp::GetRemainingAttempts(const QByteArray& login, qint64* lockoutDurationSecsPtr) const
{
	const int maxFailedAttempts = *m_maxFailedAttemptsAttrPtr;
	if (maxFailedAttempts <= 0){
		return -1;
	}

	if (lockoutDurationSecsPtr != nullptr){
		const int lockoutDurationSecs = *m_lockoutDurationAttrPtr;
		*lockoutDurationSecsPtr = (lockoutDurationSecs > 0) ? lockoutDurationSecs : -1;
	}

	QMutexLocker locker(&m_mutex);

	auto iter = m_accountStates.find(login);
	if (iter == m_accountStates.end()){
		return maxFailedAttempts;
	}

	const qint64 nowSecs = QDateTime::currentSecsSinceEpoch();
	if (IsLockedInternal(*iter, nowSecs)){
		return 0;
	}

	// The same window as RegisterFailedAttempt(): older attempts no longer count
	const qint64 periodStartSecs = nowSecs - *m_failedAttemptsPeriodAttrPtr;
	int countedAttempts = 0;
	for (const qint64 attemptTime : std::as_const(iter->failedAttemptTimes)){
		if (attemptTime >= periodStartSecs){
			++countedAttempts;
		}
	}

	return qMax(0, maxFailedAttempts - countedAttempts);
}


bool CAccountLockoutControllerComp::RegisterFailedAttempt(const QByteArray& login)
{
	int maxFailedAttempts = *m_maxFailedAttemptsAttrPtr;
	if (maxFailedAttempts <= 0){
		return false;
	}

	QMutexLocker locker(&m_mutex);

	qint64 nowSecs = QDateTime::currentSecsSinceEpoch();

	AccountState& accountState = m_accountStates[login];
	if (IsLockedInternal(accountState, nowSecs)){
		return true;
	}

	// count only the consecutive invalid attempts within the configured time period
	qint64 periodStartSecs = nowSecs - *m_failedAttemptsPeriodAttrPtr;
	while (!accountState.failedAttemptTimes.isEmpty() && (accountState.failedAttemptTimes.first() < periodStartSecs)){
		accountState.failedAttemptTimes.removeFirst();
	}

	accountState.failedAttemptTimes.append(nowSecs);
	if (accountState.failedAttemptTimes.count() < maxFailedAttempts){
		return false;
	}

	accountState.isLocked = true;
	accountState.failedAttemptTimes.clear();

	int lockoutDurationSecs = *m_lockoutDurationAttrPtr;
	accountState.lockedUntil = (lockoutDurationSecs > 0) ? (nowSecs + lockoutDurationSecs) : 0;

	SendWarningMessage(0,
					QStringLiteral("Account '%1' has been locked after %2 consecutive invalid access attempts")
						.arg(QString(login))
						.arg(maxFailedAttempts),
					"CAccountLockoutControllerComp");

	return true;
}


void CAccountLockoutControllerComp::RegisterSuccessfulAttempt(const QByteArray& login)
{
	QMutexLocker locker(&m_mutex);

	auto iter = m_accountStates.find(login);
	if ((iter != m_accountStates.end()) && !iter->isLocked){
		m_accountStates.erase(iter);
	}
}


bool CAccountLockoutControllerComp::UnlockAccount(const QByteArray& login)
{
	QMutexLocker locker(&m_mutex);

	auto iter = m_accountStates.find(login);
	if (iter == m_accountStates.end()){
		return false;
	}

	bool wasLocked = iter->isLocked;
	m_accountStates.erase(iter);

	if (wasLocked){
		SendInfoMessage(0,
						QStringLiteral("Account '%1' has been unlocked by an administrator").arg(QString(login)),
						"CAccountLockoutControllerComp");
	}

	return true;
}


// private methods

bool CAccountLockoutControllerComp::IsLockedInternal(AccountState& accountState, qint64 nowSecs, qint64* remainingLockoutSecsPtr) const
{
	if (!accountState.isLocked){
		return false;
	}

	if (accountState.lockedUntil <= 0){
		// locked until unlocked by an administrator
		if (remainingLockoutSecsPtr != nullptr){
			*remainingLockoutSecsPtr = -1;
		}

		return true;
	}

	if (nowSecs >= accountState.lockedUntil){
		// lockout period has expired
		accountState.isLocked = false;
		accountState.lockedUntil = 0;
		accountState.failedAttemptTimes.clear();

		return false;
	}

	if (remainingLockoutSecsPtr != nullptr){
		*remainingLockoutSecsPtr = accountState.lockedUntil - nowSecs;
	}

	return true;
}


} // namespace imtauth


