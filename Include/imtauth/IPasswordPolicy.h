// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QStringList>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtauth
{


/**
	Interface for a configurable password policy providing password strength,
	password history (reuse) and password lifetime checks according to
	IEC 62443-4-2 CR 1.7 (SL-C 1-4).
	\ingroup User
*/
class IPasswordPolicy: virtual public istd::IPolymorphic
{
public:
	/**
		Lifetime status of the current password of a user.
	*/
	enum LifetimeStatus
	{
		/**
			Password is valid, no action needed.
		*/
		LS_OK,

		/**
			Password is valid, but will expire soon and the user should be prompted to change it.
		*/
		LS_EXPIRES_SOON,

		/**
			Password exceeded the maximum lifetime and must be changed.
		*/
		LS_EXPIRED,

		/**
			Password change was explicitly requested (e.g. after admin reset).
		*/
		LS_CHANGE_REQUIRED
	};

	/**
		Validate the strength of a plain-text password candidate.
		\param login Login of the user, used to reject passwords equal to the login.
		\param password Plain-text password candidate.
		\param violatedRuleIds Machine-readable IDs of the violated rules
			("MinLength", "MaxLength", "LowercaseRequired", "UppercaseRequired",
			"DigitRequired", "SpecialCharRequired", "LoginAsPassword", "BlocklistedPassword"),
			empty if the password is accepted.
		\return \c true if the password fulfills all configured strength rules.
	*/
	virtual bool ValidatePasswordStrength(const QByteArray& login, const QString& password, QStringList& violatedRuleIds) const = 0;

	/**
		Check if the password hash was already used within the configured number of password generations.
		\param userInfo User whose current password hash and password history are checked.
		\param passwordHash Hash of the password candidate.
		\return \c true if the password was already used and thus may not be reused.
	*/
	virtual bool IsPasswordReused(const IUserInfo& userInfo, const QByteArray& passwordHash) const = 0;

	/**
		Check if the password of the user may be changed now with respect to the minimum password age.
		\return \c true if the password change is allowed.
	*/
	virtual bool IsPasswordChangeAllowed(const IUserInfo& userInfo) const = 0;

	/**
		Evaluate the lifetime status of the current password of the user.
		\param userInfo User whose password lifetime is evaluated.
		\param daysUntilExpiration Number of days until the password expires,
			only valid for \c LS_OK and \c LS_EXPIRES_SOON if a maximum lifetime is configured, otherwise -1.
	*/
	virtual LifetimeStatus GetPasswordLifetimeStatus(const IUserInfo& userInfo, int& daysUntilExpiration) const = 0;

	/**
		Get the configured number of password generations that may not be reused.
	*/
	virtual int GetPasswordHistoryDepth() const = 0;

	/**
		Update password history and password change timestamp of the user after a password change.
		The previous password hash is prepended to the history and the history
		is trimmed to the configured number of generations.
		\param userInfo User whose password was changed.
		\param previousPasswordHash Password hash that was replaced, may be empty for a newly created user.
	*/
	virtual void OnPasswordChanged(IUserInfo& userInfo, const QByteArray& previousPasswordHash) const = 0;
};


typedef istd::TUniqueInterfacePtr<IPasswordPolicy> IPasswordPolicyUniquePtr;
typedef istd::TSharedInterfacePtr<IPasswordPolicy> IPasswordPolicySharedPtr;


} // namespace imtauth


