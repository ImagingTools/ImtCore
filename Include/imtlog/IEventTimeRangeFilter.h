#pragma once


// Qt includes
#include <QtCore/QDateTime.h>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtlog
{


class IEventTimeRangeFilter: virtual public istd::IPolymorphic
{
public:
	struct TimeRange
	{
		QDateTime beginTime;
		QDateTime endTime;
	};

	virtual TimeRange GetEventTimeRangeFilter() const = 0;
	virtual bool SetEventTimeRangeFilter(const TimeRange& timeRange) = 0;
	virtual void ClearEventTimeRangeFilter() = 0;
};


} // namespace imtlog


