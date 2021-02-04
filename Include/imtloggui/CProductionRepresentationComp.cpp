#include <imtloggui/CProductionRepresentationComp.h>


namespace imtloggui
{


// public methods

CProductionRepresentationComp::CProductionRepresentationComp()
	:m_maxCount(0)
{
}


void CProductionRepresentationComp::SetGranularity(quint64 granularity)
{
	m_granularity = granularity;
	ClearStatistics();
}


quint64 CProductionRepresentationComp::GetGranularity() const
{
	return m_granularity;
}


const CProductionRepresentationComp::Timeline& CProductionRepresentationComp::GetTimeline() const
{
	return m_timeline;
}


quint64 CProductionRepresentationComp::GetMaxCount() const
{
	return m_maxCount;
}


const CProductionRepresentationComp::MaxCounters& CProductionRepresentationComp::GetMaxCounters() const
{
	return m_maxCounters;
}


void CProductionRepresentationComp::ClearStatistics()
{
	m_timeline.clear();
	m_maxCount = 0;
	m_maxCounters = MaxCounters();
}


void CProductionRepresentationComp::AddMessage(const ilog::IMessageConsumer::MessagePtr& message)
{
	if (message->GetInformationId() != 19780000){
		return;
	}

	qint64 timeStamp = message->GetInformationTimeStamp().toMSecsSinceEpoch();

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

	switch (message->GetInformationCategory()){
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


qint64 CProductionRepresentationComp::CalculateIntervalBeginTime(qint64 timeStamp)
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


// reimplemented (istd::IChangeable)

bool CProductionRepresentationComp::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const CProductionRepresentationComp* sourcePtr = dynamic_cast<const CProductionRepresentationComp*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier notifier(this);

		m_maxCount = sourcePtr->m_maxCount;
		m_maxCounters = sourcePtr->m_maxCounters;
		m_granularity = sourcePtr->m_granularity;
		m_timeline = sourcePtr->m_timeline;

		return true;
	}

	return false;
}


} // namespace imtloggui


