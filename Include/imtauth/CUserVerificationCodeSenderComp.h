// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IUserVerificationCodeSender.h>
#include <imtmail/ISmtpClient.h>
#include <imtmail/ISmtpMessageCreator.h>


namespace imtauth
{


class CUserVerificationCodeSenderComp: public icomp::CComponentBase,  virtual public IUserVerificationCodeSender
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUserVerificationCodeSenderComp);
		I_REGISTER_INTERFACE(IUserVerificationCodeSender);
		I_ASSIGN(m_smtpClientCompPtr, "SmtpClient", "SMTP client", false, "SmtpClient");
		I_ASSIGN(m_smtpMessageCreatorCompPtr, "SmtpMessageCreator", "SMTP message creator", false, "SmtpMessageCreator");
	I_END_COMPONENT;

	// reimplemented (imtauth::IUserVerificationCodeSender)
	virtual bool SendCode(const IUserInfo& userInfo, const QByteArray& verificationCode) const override;

protected:
	I_REF(imtmail::ISmtpClient, m_smtpClientCompPtr);
	I_REF(imtmail::ISmtpMessageCreator, m_smtpMessageCreatorCompPtr);
};


} // namespace imtauth


