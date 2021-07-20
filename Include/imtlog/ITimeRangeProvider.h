#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtlog/CTimeRange.h>


namespace imtlog
{


class ITimeRangeProvider: virtual public istd::IChangeable
{
public:
	virtual CTimeRange GetTimeRange() const = 0;
};


} // namespace imtlog


