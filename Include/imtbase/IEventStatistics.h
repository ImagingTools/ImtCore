#pragma once


// STL includes
#include <vector>

// ACF includes
#include <istd/IChangeable.h>

// Qt includes
#include <QtCore/QDateTime>


namespace imtbase
{


class IEventStatistics: virtual public istd::IChangeable
{
public:
	struct EventsInfo
	{
		quint64 count;
		quint64 oks;
		quint64 warnings;
		quint64 noks;

		/**
			Number of system errors (or not processed objects)
		*/
		quint64 errors;

		EventsInfo()
			:count(0),
			oks(0),
			warnings(0),
			noks(0),
			errors(0)
		{
		}
	};

	struct StatisticsItem
	{
		EventsInfo info;
		qint64 time;
	};

	typedef std::vector<StatisticsItem> StatisticsTimeline;

	/**
		Get statistics item count.
	*/
	virtual quint64 GetStatisticsItemCount() const = 0;

	/**
		Get statistics item.
	*/
	virtual const StatisticsItem& GetStatisticsItem(quint64 index) const = 0;
};


} // namespace imtbase


