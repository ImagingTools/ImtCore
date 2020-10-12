#pragma once


// Qt includes
#include <QtCore/QDateTime.h>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtlog/CTimeRange.h>


namespace imtlog
{


class IEventTimeRangeFilter: virtual public istd::IPolymorphic
{
public:
	virtual imtlog::CTimeRange GetEventTimeRangeFilter() const = 0;
	virtual bool SetEventTimeRangeFilter(const imtlog::CTimeRange & timeRange) = 0;
	virtual void ClearEventTimeRangeFilter() = 0;
};


} // namespace imtlog


