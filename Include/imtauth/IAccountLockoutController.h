// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauth
{


/**
	Controller limiting consecutive invalid authentication attempts.

	Implementations track failed login attempts per account and lock the
	account after a configurable number of consecutive invalid attempts
	within a configurable time period. A locked account is denied access
	for a configurable lockout period or until it is explicitly unlocked
	by an administrator via UnlockAccount().
*/
class IAccountLockoutController: virtual public istd::IPolymorphic
{
public:
	/**
		Check if the account identified by \c login is currently locked out.
		\param login account identifier (login name).
		\param remainingLockoutSecsPtr optional output for the remaining lockout
			time in seconds; set to a negative value if the account stays locked
			until an administrator unlocks it.
		\return \c true if access for the account must be denied.
	*/
	virtual bool IsAccountLocked(const QByteArray& login, qint64* remainingLockoutSecsPtr = nullptr) const = 0;

	/**
		Register an invalid access attempt for the account identified by \c login.
		\return \c true if the account became (or already was) locked.
	*/
	virtual bool RegisterFailedAttempt(const QByteArray& login) = 0;

	/**
		Register a successful access attempt for the account identified by \c login,
		resetting its consecutive failed attempt counter.
	*/
	virtual void RegisterSuccessfulAttempt(const QByteArray& login) = 0;

	/**
		Unlock the account identified by \c login prior to the expiration of the
		lockout period. Intended to be triggered by an administrator.
		\return \c true if the account state was reset.
	*/
	virtual bool UnlockAccount(const QByteArray& login) = 0;
};


} // namespace imtauth


