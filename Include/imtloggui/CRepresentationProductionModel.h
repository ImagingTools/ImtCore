#pragma once


// Qt includes
#include <QtCore/QDateTime.h>
#include <QtCore/QMap.h>

// Acf includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/IEventStatistics.h>
#include <imtlog/IMessageHistoryConsumer.h>


namespace imtloggui
{


class CRepresentationProductionModel: virtual public istd::IChangeable
{
public:
	typedef QMap<qint64, imtbase::IEventStatistics::EventsInfo> Timeline;
	struct MaxCounters
	{
		quint64 oks;
		quint64 warnings;
		quint64 noks;
		quint64 errors;
		quint64 noksErrors;

		MaxCounters()
			:oks(0),
			warnings(0),
			noks(0),
			errors(0),
			noksErrors(0)
		{
		}
	};

	CRepresentationProductionModel(quint64 granularity);
	quint64 GetGranularity() const;
	const Timeline& GetTimeline() const;
	quint64 GetMaxCount() const;
	const MaxCounters& GetMaxCounters() const;
	void ClearStatistics();
	void AddMessage(const imtlog::IMessageHistoryConsumer::Message& message);
	qint64 CalculateIntervalBeginTime(qint64 timeStamp);

private:
	quint64 m_maxCount;
	MaxCounters m_maxCounters;
	qint64 m_granularity;
	qint64 m_beginTimeStamp;
	Timeline m_timeline;
};


} // namespace imtloggui


