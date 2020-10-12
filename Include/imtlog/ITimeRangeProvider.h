#pragma once


// Qt includes
#include <QtCore/QDateTime.h>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtlog/CTimeRange.h>


namespace imtlog
{


class ITimeRangeProvider: virtual public istd::IPolymorphic
{
public:
	virtual CTimeRange GetTimeRange() const = 0;
};


} // namespace imtlog


