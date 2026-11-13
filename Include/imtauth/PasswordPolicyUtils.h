// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtauth/IPasswordPolicy.h>
#include <imtauth/IUserInfo.h>


namespace imtauth
{


/**
	Store the hash of a new password and record the change: the previous hash goes to the
	password history and the change time is set, so the password lifetime starts now.

	\param passwordPolicyPtr Optional policy; without it only the change time is recorded.
*/
inline void StoreNewPasswordHash(IUserInfo& userInfo, const QByteArray& passwordHash, const IPasswordPolicy* passwordPolicyPtr)
{
	const QByteArray previousPasswordHash = userInfo.GetPasswordHash();
	userInfo.SetPasswordHash(passwordHash);

	if (passwordPolicyPtr != nullptr){
		passwordPolicyPtr->OnPasswordChanged(userInfo, previousPasswordHash);
	}
	else{
		userInfo.SetPasswordChangedAt(QDateTime::currentDateTimeUtc());
		userInfo.SetMustChangePassword(false);
	}
}


/**
	Check a new password against the policy: strength and reuse.
	The minimum password age is not checked here: it applies only to a change by the user.

	\param passwordPolicyPtr Optional policy; without it every password is accepted.
	\param violatedRuleIds Receives the IDs of the violated rules.
	\return false if the policy refuses the password.
*/
inline bool CheckNewPassword(
			const IPasswordPolicy* passwordPolicyPtr,
			const IUserInfo& userInfo,
			const QByteArray& login,
			const QString& password,
			const QByteArray& passwordHash,
			QStringList& violatedRuleIds)
{
	if (passwordPolicyPtr == nullptr){
		return true;
	}

	if (!passwordPolicyPtr->ValidatePasswordStrength(login, password, violatedRuleIds)){
		return false;
	}

	if (passwordPolicyPtr->IsPasswordReused(userInfo, passwordHash)){
		violatedRuleIds << QStringLiteral("PasswordReused");

		return false;
	}

	return true;
}


} // namespace imtauth


