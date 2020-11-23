#include <imtloggui/CRepresentationProductionSpeedFactoryComp.h>


// ACF includes
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtloggui/CRepresentationProductionSpeedModel.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IRepresentationFactory)

IRepresentationFactory::RepresentationObjectPtr CRepresentationProductionSpeedFactoryComp::CreateRepresentationObject(
			const imtlog::CTimeRange& timeRange,
			const QList<int>& messageIdList,
			imtlog::IEventMessageIdFilter::Mode mode) const
{
	if (m_messageContainerCompPtr.IsValid() && m_timeRangeFilterCompPtr.IsValid() && m_messageIdFilterCompPtr.IsValid()){
		m_timeRangeFilterCompPtr->SetEventTimeRangeFilter(timeRange);
		m_messageIdFilterCompPtr->SetEventMessageIdFilterMode(imtlog::IEventMessageIdFilter::M_ACCEPT);
		m_messageIdFilterCompPtr->SetEventMessageIdFilter({19780000});

		imtlog::IMessageHistoryContainer::Messages messages = m_messageContainerCompPtr->GetMessages();

		CRepresentationProductionSpeedModel* modelPtr =
					const_cast<CRepresentationProductionSpeedModel*>(
								dynamic_cast<const CRepresentationProductionSpeedModel*>(m_modelPtr.GetPtr()));
		
		modelPtr->ClearStatistics();
		if (messages.isEmpty()){
			return m_modelPtr;
		}

		QDateTime dt = messages.front().messagePtr->GetInformationTimeStamp();
		qint64 beginTime = dt.toMSecsSinceEpoch();
		qint64 endTime = messages.back().messagePtr->GetInformationTimeStamp().toMSecsSinceEpoch();
		qint64 granularity = *m_granularityAttrPtr * 1000;
		beginTime += -(beginTime % granularity);
		endTime += -(endTime % granularity) + granularity;

		Q_ASSERT((endTime - beginTime) % granularity == 0);
		qint64 count = (endTime - beginTime) / granularity;

		imtlog::IMessageHistoryContainer::Messages::const_iterator it = messages.constBegin();

		for (int i = 0; i < count; i++){
			imtbase::IEventStatistics::StatisticsItem item;
			int64_t itemBeginTime = beginTime + granularity * i;
			int64_t itemEndTime = itemBeginTime + granularity;
			item.time = itemBeginTime;

			while (it != messages.end() && it->messagePtr->GetInformationTimeStamp().toMSecsSinceEpoch() < itemEndTime){
				switch (it->messagePtr->GetInformationCategory()){
				case istd::IInformationProvider::IC_INFO:
					item.info.oks++;
					item.info.count++;
					break;
				case istd::IInformationProvider::IC_WARNING:
					item.info.warnings++;
					item.info.count++;
					break;
				case istd::IInformationProvider::IC_ERROR:
					item.info.noks++;
					item.info.count++;
					break;
				case istd::IInformationProvider::IC_NONE:
				case istd::IInformationProvider::IC_CRITICAL:
					item.info.errors++;
					item.info.count++;
					break;
				}

				it++;
			}

			modelPtr->AddStatisticsItem(item);
		}
	}

	return istd::TSmartPtr<istd::IChangeable>(m_modelPtr);
}


void CRepresentationProductionSpeedFactoryComp::OnComponentCreated()
{
	m_modelPtr = istd::TSmartPtr<istd::IChangeable>(new CRepresentationProductionSpeedModel(*m_granularityAttrPtr * 1000));
}


} // namespace imtloggui


