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


const CRepresentationProductionSpeedModel::MaxCounters& CRepresentationProductionSpeedModel::GetMaxCounters() const
{
	return m_maxCounters;
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

	Q_ASSERT(m_granularity > 0);

	if (m_timeline.size() > 0){
		Q_ASSERT(m_timeline.lastKey() <= timeStamp);
	}

	if (m_timeline.size() == 0){
		imtbase::IEventStatistics::EventsInfo item;
		m_timeline[timeStamp] = item;
	}
	else{
		qint64 beginTime = CalculateIntervalBeginTime(timeStamp);
		if (m_timeline.lastKey() != beginTime){
			imtbase::IEventStatistics::EventsInfo item;
			m_timeline[beginTime] = item;
		}
	}

	imtbase::IEventStatistics::EventsInfo& info = m_timeline.last();

	switch (message.messagePtr->GetInformationCategory()){
	case istd::IInformationProvider::IC_INFO:
		info.oks++;
		info.count++;
		if (m_maxCounters.oks < info.oks){
			m_maxCounters.oks = info.oks;
		}
		break;
	case istd::IInformationProvider::IC_WARNING:
		info.warnings++;
		info.count++;
		if (m_maxCounters.warnings < info.warnings){
			m_maxCounters.warnings = info.warnings;
		}
		break;
	case istd::IInformationProvider::IC_ERROR:
		info.noks++;
		info.count++;
		if (m_maxCounters.noks < info.noks){
			m_maxCounters.noks = info.noks;
		}
		break;
	case istd::IInformationProvider::IC_NONE:
	case istd::IInformationProvider::IC_CRITICAL:
		info.errors++;
		info.count++;
		if (m_maxCounters.errors < info.errors){
			m_maxCounters.errors = info.errors;
		}
		break;
	}

	if (m_maxCount < info.count){
		m_maxCount = info.count;
	}

	if (m_maxCounters.noksErrors < info.noks + info.errors){
		m_maxCounters.noksErrors = info.noks + info.errors;
	}
}


qint64 CRepresentationProductionSpeedModel::CalculateIntervalBeginTime(qint64 timeStamp)
{
	if (m_timeline.empty()){
		return timeStamp;
	}

	qint64 beginG = timeStamp - timeStamp % m_granularity;
	qint64 deltaXG = m_timeline.firstKey() % m_granularity;
	qint64 beginX = beginG + deltaXG;
	if (timeStamp < beginX){
		beginX -= m_granularity;
	}

	return beginX;
}


} // namespace imtloggui


