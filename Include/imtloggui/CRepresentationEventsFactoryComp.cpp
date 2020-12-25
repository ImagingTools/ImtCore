#include <imtloggui/CRepresentationEventsFactoryComp.h>


// Acf includes
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtloggui/CRepresentationEventsObject.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IRepresentationFactory)

IRepresentationFactory::RepresentationObjectPtr CRepresentationEventsFactoryComp::CreateRepresentationObject(
			const imtlog::CTimeRange& timeRange,
			const QList<int>& messageIdList,
			imtlog::IMessageFilter::FilterMode filterMode) const
{
	CRepresentationEventsObject* retVal = new CRepresentationEventsObject();

	if (m_messageHistoryProviderCompPtr.IsValid()){
		Filter filter(timeRange, messageIdList, filterMode);

		imtlog::IMessageHistoryConsumer::Messages messages = m_messageHistoryProviderCompPtr->GetMessages(&filter);
		retVal->append(messages);
	}

	return istd::TSmartPtr<istd::IChangeable>(retVal);
}


// public methods of the embedded class Filter

// reimplemented (imtlog::IMessageFilter)

bool CRepresentationEventsFactoryComp::Filter::IsMessageAccepted(const istd::IInformationProvider* messagePtr) const
{
	if (m_timeRange.IsValid() && !m_timeRange.Contains(messagePtr->GetInformationTimeStamp())){
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

imtlog::CTimeRange CRepresentationEventsFactoryComp::Filter::GetTimeRange() const
{
	return m_timeRange;
}


} // namespace imtloggui


