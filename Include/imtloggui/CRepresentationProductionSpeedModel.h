#pragma once


// Qt includes
#include <QtCore/QDateTime.h>
#include <QtCore/QMap.h>

// Acf includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/IEventStatistics.h>
#include <imtlog/IMessageHistoryConsumer.h>
#include <imtlog/IMessageHistoryContainer.h>


namespace imtloggui
{


class CRepresentationProductionSpeedModel: virtual public istd::IChangeable
{
public:
	typedef QMap<qint64, imtbase::IEventStatistics::EventsInfo> Timeline;

	CRepresentationProductionSpeedModel(quint64 granularity);
	quint64 GetGranularity() const;
	const Timeline& GetTimeline() const;
	quint64 GetMaxCount() const;
	void ClearStatistics();
	void AddMessage(const imtlog::IMessageHistoryConsumer::Message& message);
	
private:
	quint64 m_maxCount;
	quint64 m_granularity;
	Timeline m_timeline;
};


} // namespace imtloggui


