#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtbase/CTimeRange.h>


namespace imtbase
{


class ITimeRangeProvider: virtual public istd::IChangeable
{
public:
	virtual imtbase::CTimeRange GetTimeRange() const = 0;
};


} // namespace imtbase


