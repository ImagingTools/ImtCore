#pragma once


// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtlog
{


class IMessageFilterParams: virtual public istd::IPolymorphic
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
};


} // namespace imtlog


