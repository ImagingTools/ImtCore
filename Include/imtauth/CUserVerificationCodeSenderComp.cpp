// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserVerificationCodeSenderComp.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IUserVerificationCodeSender)

bool CUserVerificationCodeSenderComp::SendCode(const IUserInfo& userInfo, const QByteArray& verificationCode) const
{
	if (!m_smtpMessageCreatorCompPtr.IsValid()){
		return false;
	}

	if (!m_smtpClientCompPtr.IsValid()){
		return false;
	}

	istd::TDelPtr<imtmail::ISmtpMessage> smtpMessagePtr = m_smtpMessageCreatorCompPtr->CreateMessage();
	if (!smtpMessagePtr.IsValid()){
		return false;
	}

	smtpMessagePtr->SetSubject("Imaging Tools: Password Recovery");

	QString message = QString("Your code to restore access to the '%1' account : %2").arg(userInfo.GetName(), qPrintable(verificationCode));
	smtpMessagePtr->SetBody(message);

	QString email = userInfo.GetMail();
	smtpMessagePtr->SetTo(email);

	return m_smtpClientCompPtr->SendEmail(*smtpMessagePtr);
}


} // namespace imtauth

