// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtmail/CSmtpMessageCreatorComp.h>


// ImtCore includes
#include <imtmail/CSmtpMessage.h>


namespace imtmail
{


// protected methods

// reimplemented (imtmail::ISmtpMessageCreator)

ISmtpMessage* CSmtpMessageCreatorComp::CreateMessage() const
{
	istd::TDelPtr<ISmtpMessage> smtpMessagePtr;
	smtpMessagePtr.SetPtr(new CSmtpMessage);

	QString from;
	if (m_fromAttrPtr.IsValid()){
		from = *m_fromAttrPtr;
	}

	if (m_fromParamCompPtr.IsValid()){
		from = m_fromParamCompPtr->GetText();
	}

	smtpMessagePtr->SetFrom(from);

	QString to;
	if (m_toAttrPtr.IsValid()){
		to = *m_toAttrPtr;
	}

	if (m_toParamCompPtr.IsValid()){
		to = m_toParamCompPtr->GetText();
	}

	smtpMessagePtr->SetTo(to);

	QString subject;
	if (m_subjectAttrPtr.IsValid()){
		subject = *m_subjectAttrPtr;
	}

	if (m_subjectParamCompPtr.IsValid()){
		subject = m_subjectParamCompPtr->GetText();
	}

	smtpMessagePtr->SetSubject(subject);

	QString body;
	if (m_bodyAttrPtr.IsValid()){
		body = *m_bodyAttrPtr;
	}

	if (m_bodyParamCompPtr.IsValid()){
		body = m_bodyParamCompPtr->GetText();
	}

	smtpMessagePtr->SetBody(body);

	return smtpMessagePtr.PopPtr();
}


} // namespace imtmail
