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
	enum TimeUnit
	{
		TU_CUSTOM,
		TU_HOUR,
		TU_DAY,
		TU_WEEK,
		TU_MONTH,
		TU_YEAR
	};

	I_DECLARE_ENUM(TimeUnit, TU_CUSTOM, TU_HOUR, TU_DAY, TU_WEEK, TU_MONTH, TU_YEAR);

	enum InterpretationMode
	{
		/**
			Something is present for a time unit
		*/
		IM_FOR,

		/**
			Something is present within the current time unit
		*/
		IM_CURRENT,

		/**
			Something is present within the previous time unit.
		*/
		IM_LAST
	};

	I_DECLARE_ENUM(InterpretationMode, IM_FOR, IM_CURRENT, IM_LAST);

	virtual imtbase::CTimeRange GetTimeRange() const = 0;
	virtual void SetTimeRange(const imtbase::CTimeRange& timeRange) = 0;
	virtual TimeUnit GetTimeUnit() const = 0;
	virtual void SetTimeUnit(TimeUnit timeUnit, InterpretationMode interpretationMode, int multiplier = 1) = 0;
	virtual InterpretationMode GetInterpretationMode() const = 0;
	virtual int GetUnitMultiplier() const = 0;
};


} // namespace imtbase


