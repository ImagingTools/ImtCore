#include <imtloggui/CRepresentationProductionSpeedModel.h>


namespace imtloggui
{


// public methods

CRepresentationProductionSpeedModel::CRepresentationProductionSpeedModel(uint64_t granularity)
	:m_granularity(granularity)
{
}


uint64_t CRepresentationProductionSpeedModel::GetGranularity()
{
	return m_granularity;
}


void CRepresentationProductionSpeedModel::ClearStatistics()
{
	m_timeline.clear();
}


void CRepresentationProductionSpeedModel::AddStatisticsItem(const StatisticsItem& item)
{
	if (m_timeline.size() > 0){
		Q_ASSERT(m_timeline.back().time + m_granularity == item.time);
		m_timeline.push_back(item);
	}
	else{
		m_timeline.push_back(item);
	}
}


// reimplemented (imtbase::IEventStatistics)

quint64 CRepresentationProductionSpeedModel::GetStatisticsItemCount() const
{
	return m_timeline.size();
}


const imtbase::IEventStatistics::StatisticsItem& CRepresentationProductionSpeedModel::GetStatisticsItem(uint64_t index) const
{
	return m_timeline.at(index);
}


} // namespace imtloggui


