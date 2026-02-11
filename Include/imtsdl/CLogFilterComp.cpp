// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdl/CLogFilterComp.h>


// ACF includes
#include <ilog/CMessage.h>


namespace imtsdl
{


// public methods

// reimplemented (ilog::IMessageConsumer)

bool CLogFilterComp::IsMessageSupported(
			int messageCategory,
			int messageId,
			const istd::IInformationProvider* messagePtr) const
{
	bool isGenerationMode = !m_argumentParserCompPtr.IsValid() || m_argumentParserCompPtr->IsGenerateMode();
	if (isGenerationMode){
		return m_logCompPtr->IsMessageSupported(messageCategory, messageId, messagePtr);
	}

	// If dependency mode is enabled, we should only display error messages so as not to display informational messages, as this may be perceived as a found dependency.
	switch (messageCategory){
	case istd::IInformationProvider::IC_ERROR:
	case istd::IInformationProvider::IC_CRITICAL:
		return true;

	default:
		return false;
	}

	I_CRITICAL();
	return true;
}


void CLogFilterComp::AddMessage(const MessagePtr& messagePtr)
{
	if (m_logCompPtr.IsValid()){
		if (IsMessageSupported(messagePtr->GetInformationCategory(), messagePtr->GetInformationId(), nullptr)){
			m_logCompPtr->AddMessage(messagePtr);
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CLogFilterComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	Q_ASSERT_X(m_logCompPtr.IsValid(), "Log is not set!", __func__);
	Q_ASSERT_X(m_argumentParserCompPtr.IsValid(), "ArgumentParser is not set!", __func__);
}


} // namespace imtsdl


