#pragma once


// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtlog/CTimeRange.h>


namespace imtlog
{


class IMessageFilterParams: virtual public istd::IChangeable
{
public:
	enum FilterMode
	{
		FM_INCLUDE = 0,
		FM_EXCLUDE
	};

	virtual FilterMode GetFilterMode() const = 0;
	virtual void SetFilterMode(FilterMode mode) = 0;

	virtual QSet<int> GetMessageFilterIds() const = 0;
	virtual void SetMessageFilterIds(const QSet<int>& ids) = 0;

	virtual imtlog::CTimeRange GetFilterTimeRange() const = 0;
	virtual void SetFilterTimeRange(const imtlog::CTimeRange timeRange) = 0;
};


} // namespace imtlog


