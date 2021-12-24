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
	virtual const imtbase::CTimeRange& GetTimeRange() const = 0;
	virtual void SetTimeRange(const imtbase::CTimeRange& timeRange) = 0;
};


} // namespace imtbase


