#pragma once


// Qt includes
#include <QtCore/QDateTime.h>

// Acf includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/IEventStatistics.h>


namespace imtloggui
{


class CRepresentationProductionSpeedModel:
			virtual public istd::IChangeable,
			virtual public imtbase::IEventStatistics
{
public:
	typedef istd::IChangeable BaseClass;

	CRepresentationProductionSpeedModel(uint64_t granularity);
	uint64_t GetGranularity();
	void ClearStatistics();
	void AddStatisticsItem(const StatisticsItem& item);

	// reimplemented (imtbase::IEventStatistics)
	virtual uint64_t GetStatisticsItemCount() const override;
	virtual const StatisticsItem& GetStatisticsItem(uint64_t index) const override;

private:
	uint64_t m_granularity;
	std::vector<StatisticsItem> m_timeline;
};


} // namespace imtloggui


