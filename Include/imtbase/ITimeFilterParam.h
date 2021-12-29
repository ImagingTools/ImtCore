#pragma once


// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtbase/CTimeRange.h>


namespace imtbase
{


class ITimeFilterParam: virtual public iser::ISerializable
{
public:
	enum TimeInterval
	{
		TI__CUSTOM,
		TI_THIS_WEEK,
		TI_WEEK_AGO,
		TI_LAST_WEEK,
		TI_THIS_MONTH,
		TI_MONTH_AGO,
		TI_LAST_MONTH,
		TI_THIS_YEAR,
		TI_LAST_YEAR,
		TI_YEAR_AGO
	};

	I_DECLARE_ENUM(TimeInterval, TI__CUSTOM, TI_THIS_WEEK, TI_WEEK_AGO, TI_LAST_WEEK, TI_THIS_MONTH, TI_MONTH_AGO, TI_LAST_MONTH, TI_THIS_YEAR, TI_LAST_YEAR, TI_YEAR_AGO);

	virtual const imtbase::CTimeRange& GetTimeRange() const = 0;
	virtual void SetTimeRange(const imtbase::CTimeRange& timeRange) = 0;
	virtual TimeInterval GetTimeInterval() const = 0;
	virtual void SetTimeInterval(TimeInterval timeInterval) = 0;
};


} // namespace imtbase


