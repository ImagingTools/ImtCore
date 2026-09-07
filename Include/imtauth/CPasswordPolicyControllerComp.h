// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IPasswordPolicy.h>


namespace imtauth
{


/**
	Configurable password policy component implementing password strength,
	password history (reuse) and password lifetime checks according to
	IEC 62443-4-2 CR 1.7 (SL-C 1-4).

	All lifetime restrictions are disabled by default (attribute value 0),
	so existing deployments are not affected unless explicitly configured.
*/
class CPasswordPolicyControllerComp:
			public icomp::CComponentBase,
			virtual public imtauth::IPasswordPolicy
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CPasswordPolicyControllerComp);
		I_REGISTER_INTERFACE(imtauth::IPasswordPolicy);
		I_ASSIGN(m_minPasswordLengthAttrPtr, "MinPasswordLength", "Minimum password length", false, 8);
		I_ASSIGN(m_maxPasswordLengthAttrPtr, "MaxPasswordLength", "Maximum password length (0 = unlimited)", false, 128);
		I_ASSIGN(m_requireLowercaseAttrPtr, "RequireLowercase", "Require at least one lowercase letter", false, false);
		I_ASSIGN(m_requireUppercaseAttrPtr, "RequireUppercase", "Require at least one uppercase letter", false, false);
		I_ASSIGN(m_requireDigitAttrPtr, "RequireDigit", "Require at least one digit", false, false);
		I_ASSIGN(m_requireSpecialCharAttrPtr, "RequireSpecialChar", "Require at least one special character", false, false);
		I_ASSIGN(m_rejectLoginAsPasswordAttrPtr, "RejectLoginAsPassword", "Reject passwords equal to the login", false, true);
		I_ASSIGN(m_blocklistFilePathAttrPtr, "BlocklistFilePath", "Path to a text file with blocked passwords (one per line)", false, "");
		I_ASSIGN(m_passwordHistoryDepthAttrPtr, "PasswordHistoryDepth", "Number of previous password generations that may not be reused (0 = disabled)", false, 5);
		I_ASSIGN(m_minPasswordAgeAttrPtr, "MinPasswordAge", "Minimum password age in days before the password may be changed again (0 = disabled)", false, 0);
		I_ASSIGN(m_maxPasswordAgeAttrPtr, "MaxPasswordAge", "Maximum password age in days before the password expires (0 = disabled)", false, 0);
		I_ASSIGN(m_expirationWarningPeriodAttrPtr, "ExpirationWarningPeriod", "Number of days before expiration to prompt the user to change the password (0 = disabled)", false, 14);
	I_END_COMPONENT;

	// reimplemented (imtauth::IPasswordPolicy)
	virtual bool ValidatePasswordStrength(const QByteArray& login, const QString& password, QStringList& violatedRuleIds) const override;
	virtual bool IsPasswordReused(const IUserInfo& userInfo, const QByteArray& passwordHash) const override;
	virtual bool IsPasswordChangeAllowed(const IUserInfo& userInfo) const override;
	virtual LifetimeStatus GetPasswordLifetimeStatus(const IUserInfo& userInfo, int& daysUntilExpiration) const override;
	virtual int GetPasswordHistoryDepth() const override;
	virtual void OnPasswordChanged(IUserInfo& userInfo, const QByteArray& previousPasswordHash) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(int, m_minPasswordLengthAttrPtr);
	I_ATTR(int, m_maxPasswordLengthAttrPtr);
	I_ATTR(bool, m_requireLowercaseAttrPtr);
	I_ATTR(bool, m_requireUppercaseAttrPtr);
	I_ATTR(bool, m_requireDigitAttrPtr);
	I_ATTR(bool, m_requireSpecialCharAttrPtr);
	I_ATTR(bool, m_rejectLoginAsPasswordAttrPtr);
	I_ATTR(QString, m_blocklistFilePathAttrPtr);
	I_ATTR(int, m_passwordHistoryDepthAttrPtr);
	I_ATTR(int, m_minPasswordAgeAttrPtr);
	I_ATTR(int, m_maxPasswordAgeAttrPtr);
	I_ATTR(int, m_expirationWarningPeriodAttrPtr);

private:
	QSet<QString> m_blockedPasswords;
};


} // namespace imtauth


