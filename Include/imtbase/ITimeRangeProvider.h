#pragma once


// ImtCore includes
#include <imtbase/CTimeRange.h>


namespace imtbase
{


class ITimeRangeProvider: virtual public istd::IChangeable
{
public:
	virtual CTimeRange GetTimeRange() const = 0;
};


} // namespace imtbase


