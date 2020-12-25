#include <imtloggui/CRepresentationProductionFactoryComp.h>


// ACF includes
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtloggui/CRepresentationProductionModel.h>


namespace imtloggui
{


// public methods

// reimplemented (ilog::IMessageConsumer)

bool CRepresentationProductionFactoryComp::IsMessageSupported(const istd::IInformationProvider* messagePtr) const
{
	return messagePtr->GetInformationId() == 19780000;
}


void CRepresentationProductionFactoryComp::AddMessage(const MessagePtr& messagePtr)
{
	if (messagePtr->messagePtr->GetInformationId() != 19780000){
		return;
	}

	CRepresentationProductionFactoryComp* modelPtr = dynamic_cast<CRepresentationProductionFactoryComp*>(m_modelPtr.GetPtr());
	Q_ASSERT(modelPtr != nullptr);

	modelPtr->AddMessage(messagePtr);
}


// reimplemented (imtloggui::IRepresentationFactory)

IRepresentationFactory::RepresentationObjectPtr CRepresentationProductionFactoryComp::CreateRepresentationObject(
			const imtlog::CTimeRange& timeRange,
			const QList<int>& messageIdList,
			imtlog::IMessageFilter::FilterMode filterMode) const
{
	return m_modelPtr;
}


// reimplemented (icomp::CComponentBase)

void CRepresentationProductionFactoryComp::OnComponentCreated()
{
	CRepresentationProductionModel* modelPtr = new CRepresentationProductionModel(*m_granularityAttrPtr * 1000);
	m_modelPtr = istd::TSmartPtr<istd::IChangeable>(modelPtr);

	//if (m_timeRangeProviderCompPtr.IsValid() && m_timeRangeFilterCompPtr.IsValid() && m_messageIdFilterCompPtr.IsValid()){
	//	imtlog::CTimeRange timeRange = m_timeRangeProviderCompPtr->GetTimeRange();

	//	m_timeRangeFilterCompPtr->SetEventTimeRangeFilter(timeRange);
	//	m_messageIdFilterCompPtr->SetEventMessageIdFilterMode(imtlog::IEventMessageIdFilter::M_ACCEPT);
	//	m_messageIdFilterCompPtr->SetEventMessageIdFilter({19780000});

	//	imtlog::IMessageHistoryContainer::Messages messages = m_messageContainerCompPtr->GetMessages();

	//	for (imtlog::IMessageHistoryConsumer::Message message : messages){
	//		modelPtr->AddMessage(message);
	//	}
	//}
}


} // namespace imtloggui


