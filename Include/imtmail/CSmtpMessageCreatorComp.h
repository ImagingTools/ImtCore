// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtmail/ISmtpMessageCreator.h>


namespace imtmail
{


class CSmtpMessageCreatorComp: public icomp::CComponentBase,  virtual public imtmail::ISmtpMessageCreator
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSmtpMessageCreatorComp);
		I_REGISTER_INTERFACE(imtmail::ISmtpMessageCreator);
		I_ASSIGN(m_fromAttrPtr, "From", "The sender of the message", false, "");
		I_ASSIGN(m_fromParamCompPtr, "FromParam", "The sender of the message parameter", false, "");
		I_ASSIGN(m_toAttrPtr, "To", "The recipient of the message", false, "");
		I_ASSIGN(m_toParamCompPtr, "ToParam", "The recipient of the message parameter", false, "");
		I_ASSIGN(m_subjectAttrPtr, "Subject", "Message title", false, "");
		I_ASSIGN(m_subjectParamCompPtr, "SubjectParam", "Message title parameter", false, "");
		I_ASSIGN(m_bodyAttrPtr, "Body", "Message body", false, "");
		I_ASSIGN(m_bodyParamCompPtr, "BodyParam", "Message body parameter", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtmail::ISmtpMessageCreator)
	virtual ISmtpMessage* CreateMessage() const override;

private:
	I_REF(iprm::ITextParam, m_fromParamCompPtr);
	I_ATTR(QString, m_fromAttrPtr);
	I_REF(iprm::ITextParam, m_toParamCompPtr);
	I_ATTR(QString, m_toAttrPtr);
	I_REF(iprm::ITextParam, m_subjectParamCompPtr);
	I_ATTR(QString, m_subjectAttrPtr);
	I_REF(iprm::ITextParam, m_bodyParamCompPtr);
	I_ATTR(QString, m_bodyAttrPtr);
};


} // namespace imtmail
