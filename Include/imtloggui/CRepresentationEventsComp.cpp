#include <imtloggui/CRepresentationEventsComp.h>


// Acf includes
#include <istd/TSmartPtr.h>
#include <istd/CChangeGroup.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IRepresentation)

imod::IModel* CRepresentationEventsComp::GetRepresentationModel()
{
	return nullptr;
}


imod::IObserver* CRepresentationEventsComp::GetTimeRangeObserver()
{
	return nullptr;
}


// reimplemented (ilog::IMessageConsumer)

bool CRepresentationEventsComp::IsMessageSupported(
	int /*messageCategory*/,
	int messageId,
	const istd::IInformationProvider* messagePtr) const
{
	return (messageId && (messagePtr->GetInformationId() == 19780000));
}


void CRepresentationEventsComp::AddMessage(const MessagePtr& messagePtr)
{
	if (messagePtr->GetInformationId() != 19780000){
		return;
	}

	//CRepresentationProductionComp* modelPtr = dynamic_cast<CRepresentationProductionComp*>(m_modelPtr.GetPtr());
	//Q_ASSERT(modelPtr != nullptr);

	//modelPtr->AddMessage(messagePtr);
}


// reimplemented (imtloggui::IRepresentationFactory)

//IRepresentation::RepresentationObjectPtr CRepresentationEventsComp::CreateRepresentationObject(
//			const imtlog::CTimeRange& timeRange,
//			const QList<int>& messageIdList,
//			imtlog::IEventFilter::FilterMode filterMode) const
//{
//
//	if (m_messageHistoryProviderCompPtr.IsValid()){
//		imtlog::IEventProvider::EventFilterPtr filterPtr =
//					imtlog::IEventProvider::EventFilterPtr(new Filter(timeRange, messageIdList, filterMode));
//
//		ilog::IMessageContainer::Messages messages = m_messageHistoryProviderCompPtr->GetEvents(filterPtr)->GetMessages();
//
//		//CRepresentationEventsObject* modelPtr = dynamic_cast<CRepresentationEventsObject*>(m_modelPtr.GetPtr());
//		//Q_ASSERT(modelPtr != nullptr);
//
//		//istd::CChangeGroup notifier(modelPtr);
//		//modelPtr->ClearMessages();
//		//for (int i = messages.count() - 1; i >= 0 ; i--){
////			modelPtr->AddMessage(messages[i]);
//		//}
//	}
//
//	return m_modelPtr;
//}


// reimplemented (icomp::CComponentBase)

void CRepresentationEventsComp::OnComponentCreated()
{
	//CRepresentationEventsObject* modelPtr = new CRepresentationEventsObject();
	//m_modelPtr = istd::TSmartPtr<istd::IChangeable>(modelPtr);
}


// public methods of the embedded class Filter

// reimplemented (imtlog::IMessageFilter)

bool CRepresentationEventsComp::Filter::IsMessageAccepted(const istd::IInformationProvider* messagePtr) const
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


QList<int> CRepresentationEventsComp::Filter::GetGroupMessageIds() const
{
	return m_messageIdList;
}


// reimplemented (imtlog::ITimeRangeProvider)

imtlog::CTimeRange CRepresentationEventsComp::Filter::GetTimeRange() const
{
	return m_timeRange;
}


} // namespace imtloggui


