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
		m_logCompPtr->AddMessage(messagePtr);
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


