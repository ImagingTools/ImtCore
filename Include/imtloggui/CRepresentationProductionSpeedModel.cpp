#include <imtloggui/CRepresentationProductionSpeedModel.h>


namespace imtloggui
{


// public methods

CRepresentationProductionSpeedModel::CRepresentationProductionSpeedModel(quint64 granularity)
	:m_maxCount(0),
	m_granularity(granularity)
{
}


quint64 CRepresentationProductionSpeedModel::GetGranularity() const
{
	return m_granularity;
}


const CRepresentationProductionSpeedModel::Timeline& CRepresentationProductionSpeedModel::GetTimeline() const
{
	return m_timeline;
}


quint64 CRepresentationProductionSpeedModel::GetMaxCount() const
{
	return m_maxCount;
}


void CRepresentationProductionSpeedModel::ClearStatistics()
{
	m_timeline.clear();
	m_maxCount = 0;
}


void CRepresentationProductionSpeedModel::AddMessage(const imtlog::IMessageHistoryConsumer::Message& message)
{
	if (message.messagePtr->GetInformationId() != 19780000){
		return;
	}

	qint64 timeStamp = message.messagePtr->GetInformationTimeStamp().toMSecsSinceEpoch();
	qint64 beginTime = timeStamp;
	beginTime -= beginTime % m_granularity;

	Q_ASSERT(m_granularity > 0);

	if (m_timeline.size() > 0){
		Q_ASSERT(m_timeline.lastKey() < timeStamp);
	}

	if (m_timeline.size() == 0){
		imtbase::IEventStatistics::EventsInfo item;
		m_timeline[beginTime] = item;
	}
	else if (m_timeline.lastKey() != beginTime){
		imtbase::IEventStatistics::EventsInfo item;
		m_timeline[beginTime] = item;
	}


	imtbase::IEventStatistics::EventsInfo& info = m_timeline.last();

	switch (message.messagePtr->GetInformationCategory()){
	case istd::IInformationProvider::IC_INFO:
		info.oks++;
		info.count++;
		break;
	case istd::IInformationProvider::IC_WARNING:
		info.warnings++;
		info.count++;
		break;
	case istd::IInformationProvider::IC_ERROR:
		info.noks++;
		info.count++;
		break;
	case istd::IInformationProvider::IC_NONE:
	case istd::IInformationProvider::IC_CRITICAL:
		info.errors++;
		info.count++;
		break;
	}

	if (m_maxCount < info.count){
		m_maxCount = info.count;
	}
}


} // namespace imtloggui


