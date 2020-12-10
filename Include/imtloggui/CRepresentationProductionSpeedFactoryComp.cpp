#include <imtloggui/CRepresentationProductionSpeedFactoryComp.h>


// ACF includes
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtloggui/CRepresentationProductionSpeedModel.h>


namespace imtloggui
{


// public methods

// reimplemented (ilog::IMessageConsumer)

bool CRepresentationProductionSpeedFactoryComp::IsMessageSupported(
			int /*messageCategory*/,
			int messageId,
			const istd::IInformationProvider* /*messagePtr*/) const
{
	return messageId == 19780000;
}


void CRepresentationProductionSpeedFactoryComp::AddMessage(const MessagePtr& messagePtr)
{
	if (messagePtr->messagePtr->GetInformationId() != 19780000){
		return;
	}

	CRepresentationProductionSpeedModel* modelPtr = dynamic_cast<CRepresentationProductionSpeedModel*>(m_modelPtr.GetPtr());
	Q_ASSERT(modelPtr != nullptr);

	modelPtr->AddMessage(*messagePtr.GetPtr());
}


// reimplemented (imtloggui::IRepresentationFactory)

IRepresentationFactory::RepresentationObjectPtr CRepresentationProductionSpeedFactoryComp::CreateRepresentationObject(
			const imtlog::CTimeRange& /*timeRange*/,
			const QList<int>& /*messageIdList*/,
			imtlog::IEventMessageIdFilter::Mode /*mode*/) const
{
	return m_modelPtr;
}


// reimplemented (icomp::CComponentBase)

void CRepresentationProductionSpeedFactoryComp::OnComponentCreated()
{
	CRepresentationProductionSpeedModel* modelPtr = new CRepresentationProductionSpeedModel(*m_granularityAttrPtr * 1000);
	m_modelPtr = istd::TSmartPtr<istd::IChangeable>(modelPtr);

	if (m_timeRangeProviderCompPtr.IsValid() && m_timeRangeFilterCompPtr.IsValid() && m_messageIdFilterCompPtr.IsValid()){
		imtlog::CTimeRange timeRange = m_timeRangeProviderCompPtr->GetTimeRange();

		m_timeRangeFilterCompPtr->SetEventTimeRangeFilter(timeRange);
		m_messageIdFilterCompPtr->SetEventMessageIdFilterMode(imtlog::IEventMessageIdFilter::M_ACCEPT);
		m_messageIdFilterCompPtr->SetEventMessageIdFilter({19780000});

		imtlog::IMessageHistoryContainer::Messages messages = m_messageContainerCompPtr->GetMessages();

		for (imtlog::IMessageHistoryConsumer::Message message : messages){
			modelPtr->AddMessage(message);
		}
	}
}


} // namespace imtloggui


