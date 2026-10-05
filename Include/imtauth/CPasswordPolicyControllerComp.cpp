// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CPasswordPolicyControllerComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTextStream>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IPasswordPolicy)

IPasswordPolicy::StrengthRules CPasswordPolicyControllerComp::GetStrengthRules() const
{
	StrengthRules retVal;

	retVal.minLength = m_minPasswordLengthAttrPtr.IsValid() ? *m_minPasswordLengthAttrPtr : PasswordPolicyDefaults::MIN_PASSWORD_LENGTH;
	retVal.maxLength = m_maxPasswordLengthAttrPtr.IsValid() ? *m_maxPasswordLengthAttrPtr : PasswordPolicyDefaults::MAX_PASSWORD_LENGTH;
	retVal.requireLowercase = m_requireLowercaseAttrPtr.IsValid() ? *m_requireLowercaseAttrPtr : PasswordPolicyDefaults::REQUIRE_LOWERCASE;
	retVal.requireUppercase = m_requireUppercaseAttrPtr.IsValid() ? *m_requireUppercaseAttrPtr : PasswordPolicyDefaults::REQUIRE_UPPERCASE;
	retVal.requireDigit = m_requireDigitAttrPtr.IsValid() ? *m_requireDigitAttrPtr : PasswordPolicyDefaults::REQUIRE_DIGIT;
	retVal.requireSpecialChar = m_requireSpecialCharAttrPtr.IsValid() ? *m_requireSpecialCharAttrPtr : PasswordPolicyDefaults::REQUIRE_SPECIAL_CHAR;
	retVal.rejectLoginAsPassword = m_rejectLoginAsPasswordAttrPtr.IsValid() ? *m_rejectLoginAsPasswordAttrPtr : PasswordPolicyDefaults::REJECT_LOGIN_AS_PASSWORD;
	retVal.blocklistUsed = !m_blockedPasswords.isEmpty();

	return retVal;
}


bool CPasswordPolicyControllerComp::ValidatePasswordStrength(const QByteArray& login, const QString& password, QStringList& violatedRuleIds) const
{
	violatedRuleIds.clear();

	StrengthRules rules = GetStrengthRules();

	if (password.length() < rules.minLength){
		violatedRuleIds << QStringLiteral("MinLength");
	}

	if (rules.maxLength > 0 && password.length() > rules.maxLength){
		violatedRuleIds << QStringLiteral("MaxLength");
	}

	bool hasLowercase = false;
	bool hasUppercase = false;
	bool hasDigit = false;
	bool hasSpecialChar = false;
	for (const QChar& character : password){
		if (character.isLower()){
			hasLowercase = true;
		}
		else if (character.isUpper()){
			hasUppercase = true;
		}
		else if (character.isDigit()){
			hasDigit = true;
		}
		else if (!character.isSpace()){
			hasSpecialChar = true;
		}
	}

	if (rules.requireLowercase && !hasLowercase){
		violatedRuleIds << QStringLiteral("LowercaseRequired");
	}

	if (rules.requireUppercase && !hasUppercase){
		violatedRuleIds << QStringLiteral("UppercaseRequired");
	}

	if (rules.requireDigit && !hasDigit){
		violatedRuleIds << QStringLiteral("DigitRequired");
	}

	if (rules.requireSpecialChar && !hasSpecialChar){
		violatedRuleIds << QStringLiteral("SpecialCharRequired");
	}

	if (rules.rejectLoginAsPassword && !login.isEmpty() && password.compare(QString(login), Qt::CaseInsensitive) == 0){
		violatedRuleIds << QStringLiteral("LoginAsPassword");
	}

	if (!m_blockedPasswords.isEmpty() && m_blockedPasswords.contains(password.toLower())){
		violatedRuleIds << QStringLiteral("BlocklistedPassword");
	}

	return violatedRuleIds.isEmpty();
}


bool CPasswordPolicyControllerComp::IsPasswordReused(const IUserInfo& userInfo, const QByteArray& passwordHash) const
{
	int historyDepth = GetPasswordHistoryDepth();
	if (historyDepth <= 0){
		return false;
	}

	if (userInfo.GetPasswordHash() == passwordHash){
		return true;
	}

	QByteArrayList passwordHistory = userInfo.GetPasswordHistory();
	for (int index = 0; index < passwordHistory.size() && index < historyDepth; index++){
		if (passwordHistory[index] == passwordHash){
			return true;
		}
	}

	return false;
}


bool CPasswordPolicyControllerComp::IsPasswordChangeAllowed(const IUserInfo& userInfo) const
{
	int minPasswordAge = m_minPasswordAgeAttrPtr.IsValid() ? *m_minPasswordAgeAttrPtr : PasswordPolicyDefaults::MIN_PASSWORD_AGE;
	if (minPasswordAge <= 0){
		return true;
	}

	QDateTime passwordChangedAt = userInfo.GetPasswordChangedAt();
	if (!passwordChangedAt.isValid()){
		return true;
	}

	if (userInfo.MustChangePassword()){
		return true;
	}

	return passwordChangedAt.addDays(minPasswordAge) <= QDateTime::currentDateTimeUtc();
}


IPasswordPolicy::LifetimeStatus CPasswordPolicyControllerComp::GetPasswordLifetimeStatus(const IUserInfo& userInfo, int& daysUntilExpiration) const
{
	daysUntilExpiration = -1;

	if (userInfo.MustChangePassword()){
		return LS_CHANGE_REQUIRED;
	}

	int maxPasswordAge = m_maxPasswordAgeAttrPtr.IsValid() ? *m_maxPasswordAgeAttrPtr : PasswordPolicyDefaults::MAX_PASSWORD_AGE;
	if (maxPasswordAge <= 0){
		return LS_OK;
	}

	QDateTime passwordChangedAt = userInfo.GetPasswordChangedAt();
	if (!passwordChangedAt.isValid()){
		// Legacy user without a stored password change timestamp;
		// expiration is enforced starting from the next password change.
		return LS_OK;
	}

	QDateTime expirationTime = passwordChangedAt.addDays(maxPasswordAge);
	QDateTime currentTime = QDateTime::currentDateTimeUtc();
	if (expirationTime <= currentTime){
		return LS_EXPIRED;
	}

	daysUntilExpiration = static_cast<int>(currentTime.daysTo(expirationTime));

	int warningPeriod = m_expirationWarningPeriodAttrPtr.IsValid() ? *m_expirationWarningPeriodAttrPtr : PasswordPolicyDefaults::EXPIRATION_WARNING_PERIOD;
	if (warningPeriod > 0 && daysUntilExpiration <= warningPeriod){
		return LS_EXPIRES_SOON;
	}

	return LS_OK;
}


int CPasswordPolicyControllerComp::GetPasswordHistoryDepth() const
{
	return m_passwordHistoryDepthAttrPtr.IsValid() ? *m_passwordHistoryDepthAttrPtr : PasswordPolicyDefaults::PASSWORD_HISTORY_DEPTH;
}


void CPasswordPolicyControllerComp::OnPasswordChanged(IUserInfo& userInfo, const QByteArray& previousPasswordHash) const
{
	int historyDepth = GetPasswordHistoryDepth();

	QByteArrayList passwordHistory = userInfo.GetPasswordHistory();
	if (historyDepth > 0 && !previousPasswordHash.isEmpty()){
		passwordHistory.prepend(previousPasswordHash);
	}

	while (passwordHistory.size() > historyDepth){
		passwordHistory.removeLast();
	}

	userInfo.SetPasswordHistory(passwordHistory);
	userInfo.SetPasswordChangedAt(QDateTime::currentDateTimeUtc());
	userInfo.SetMustChangePassword(false);
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CPasswordPolicyControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_blockedPasswords.clear();

	QString blocklistFilePath = m_blocklistFilePathAttrPtr.IsValid() ? *m_blocklistFilePathAttrPtr : QString();
	if (blocklistFilePath.isEmpty()){
		return;
	}

	QFile blocklistFile(blocklistFilePath);
	if (!blocklistFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		return;
	}

	QTextStream stream(&blocklistFile);
	while (!stream.atEnd()){
		QString blockedPassword = stream.readLine().trimmed().toLower();
		if (!blockedPassword.isEmpty()){
			m_blockedPasswords.insert(blockedPassword);
		}
	}
}


} // namespace imtauth


