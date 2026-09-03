// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CPasswordPolicyControllerComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTextStream>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IPasswordPolicy)

bool CPasswordPolicyControllerComp::ValidatePasswordStrength(const QByteArray& login, const QString& password, QStringList& violatedRuleIds) const
{
	violatedRuleIds.clear();

	int minLength = m_minPasswordLengthAttrPtr.IsValid() ? *m_minPasswordLengthAttrPtr : 8;
	if (password.length() < minLength){
		violatedRuleIds << QStringLiteral("MinLength");
	}

	int maxLength = m_maxPasswordLengthAttrPtr.IsValid() ? *m_maxPasswordLengthAttrPtr : 128;
	if (maxLength > 0 && password.length() > maxLength){
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
		else{
			hasSpecialChar = true;
		}
	}

	if (m_requireLowercaseAttrPtr.IsValid() && *m_requireLowercaseAttrPtr && !hasLowercase){
		violatedRuleIds << QStringLiteral("LowercaseRequired");
	}

	if (m_requireUppercaseAttrPtr.IsValid() && *m_requireUppercaseAttrPtr && !hasUppercase){
		violatedRuleIds << QStringLiteral("UppercaseRequired");
	}

	if (m_requireDigitAttrPtr.IsValid() && *m_requireDigitAttrPtr && !hasDigit){
		violatedRuleIds << QStringLiteral("DigitRequired");
	}

	if (m_requireSpecialCharAttrPtr.IsValid() && *m_requireSpecialCharAttrPtr && !hasSpecialChar){
		violatedRuleIds << QStringLiteral("SpecialCharRequired");
	}

	bool rejectLoginAsPassword = !m_rejectLoginAsPasswordAttrPtr.IsValid() || *m_rejectLoginAsPasswordAttrPtr;
	if (rejectLoginAsPassword && !login.isEmpty() && password.compare(QString(login), Qt::CaseInsensitive) == 0){
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
	int minPasswordAge = m_minPasswordAgeAttrPtr.IsValid() ? *m_minPasswordAgeAttrPtr : 0;
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

	int maxPasswordAge = m_maxPasswordAgeAttrPtr.IsValid() ? *m_maxPasswordAgeAttrPtr : 0;
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

	int warningPeriod = m_expirationWarningPeriodAttrPtr.IsValid() ? *m_expirationWarningPeriodAttrPtr : 0;
	if (warningPeriod > 0 && daysUntilExpiration <= warningPeriod){
		return LS_EXPIRES_SOON;
	}

	return LS_OK;
}


int CPasswordPolicyControllerComp::GetPasswordHistoryDepth() const
{
	return m_passwordHistoryDepthAttrPtr.IsValid() ? *m_passwordHistoryDepthAttrPtr : 0;
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


