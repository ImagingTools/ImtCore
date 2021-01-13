#include <imtloggui/CRepresentationProductionFactoryComp.h>


// ACF includes
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtlog/IEventProvider.h>
#include <imtloggui/CRepresentationProductionModel.h>

namespace imtloggui
{


// public methods

// reimplemented (ilog::IMessageConsumer)

bool CRepresentationProductionFactoryComp::IsMessageSupported(
			int messageCategory,
			int messageId,
			const istd::IInformationProvider* messagePtr) const
{
	return messagePtr->GetInformationId() == 19780000;
}


void CRepresentationProductionFactoryComp::AddMessage(const MessagePtr& messagePtr)
{
	if (messagePtr->GetInformationId() != 19780000){
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
			imtlog::IEventFilter::FilterMode filterMode) const
{
	return m_modelPtr;
}


// reimplemented (icomp::CComponentBase)

void CRepresentationProductionFactoryComp::OnComponentCreated()
{
	CRepresentationProductionModel* modelPtr = new CRepresentationProductionModel(*m_granularityAttrPtr * 1000);
	m_modelPtr = istd::TSmartPtr<istd::IChangeable>(modelPtr);

	imtlog::CTimeRange timeRange = m_timeRangeProviderCompPtr->GetTimeRange();

	imtlog::IEventProvider::EventFilterPtr filterPtr =
				imtlog::IEventProvider::EventFilterPtr(new Filter(m_timeRangeProviderCompPtr->GetTimeRange(), {19780000}, imtlog::IEventFilter::FM_INCLUDE));

	imtlog::IEventProvider::EventContainerPtr container = m_messageHistoryProviderCompPtr->GetEvents(filterPtr);

	for (ilog::IMessageConsumer::MessagePtr message : container->GetMessages()){
		modelPtr->AddMessage(message);
	}
}


// public methods of the embedded class Filter

// reimplemented (imtlog::IMessageFilter)

bool CRepresentationProductionFactoryComp::Filter::IsMessageAccepted(const istd::IInformationProvider* messagePtr) const
{
	if (m_timeRange.IsClosed() && !m_timeRange.Contains(messagePtr->GetInformationTimeStamp())){
		return false;
	}

	if (m_filterMode == FM_INCLUDE && !m_messageIdList.contains(messagePtr->GetInformationId())){
		return false;
	}

	if (m_filterMode == FM_EXCLUDE && m_messageIdList.contains(messagePtr->GetInformationId())){
		return false;
	}

	return true;
}


// reimplemented (imtlog::ITimeRangeProvider)

imtlog::CTimeRange CRepresentationProductionFactoryComp::Filter::GetTimeRange() const
{
	return m_timeRange;
}


} // namespace imtloggui


