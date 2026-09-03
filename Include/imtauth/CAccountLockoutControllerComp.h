// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// Qt includes
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QMutex>

// ImtCore includes
#include <imtauth/IAccountLockoutController.h>


namespace imtauth
{


/**
	Thread-safe in-memory implementation of the account lockout capability.

	Enforces a limit of a configurable number of consecutive invalid access
	attempts by any user during a configurable time period. When the limit is
	reached, access is denied for a configurable period of time or until the
	account is unlocked by an administrator via UnlockAccount().
*/
class CAccountLockoutControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::IAccountLockoutController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAccountLockoutControllerComp);
		I_REGISTER_INTERFACE(imtauth::IAccountLockoutController);
		I_ASSIGN(m_maxFailedAttemptsAttrPtr, "MaxFailedAttempts", "Maximum number of consecutive invalid access attempts before the account is locked (0 disables the lockout)", false, 5);
		I_ASSIGN(m_failedAttemptsPeriodAttrPtr, "FailedAttemptsPeriod", "Time period (in secs) within which consecutive invalid access attempts are counted", false, 5 * 60);
		I_ASSIGN(m_lockoutDurationAttrPtr, "LockoutDuration", "Time period (in secs) for which access is denied after the limit has been reached (0 locks the account until it is unlocked by an administrator)", false, 15 * 60);
	I_END_COMPONENT;

	// reimplemented (imtauth::IAccountLockoutController)
	virtual bool IsAccountLocked(const QByteArray& login, qint64* remainingLockoutSecsPtr = nullptr) const override;
	virtual bool RegisterFailedAttempt(const QByteArray& login) override;
	virtual void RegisterSuccessfulAttempt(const QByteArray& login) override;
	virtual bool UnlockAccount(const QByteArray& login) override;

private:
	struct AccountState
	{
		QList<qint64> failedAttemptTimes; // seconds since epoch
		bool isLocked = false;
		qint64 lockedUntil = 0; // seconds since epoch; 0 = locked until unlocked by an administrator
	};

	/**
		Check the lock state of an account entry, releasing an expired lock.
		Assumes m_mutex is already held by the caller.
	*/
	bool IsLockedInternal(AccountState& accountState, qint64 nowSecs, qint64* remainingLockoutSecsPtr = nullptr) const;

private:
	I_ATTR(int, m_maxFailedAttemptsAttrPtr);
	I_ATTR(int, m_failedAttemptsPeriodAttrPtr);
	I_ATTR(int, m_lockoutDurationAttrPtr);

	mutable QMutex m_mutex;
	mutable QHash<QByteArray, AccountState> m_accountStates;
};


} // namespace imtauth


