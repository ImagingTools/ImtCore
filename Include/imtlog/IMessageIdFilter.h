#pragma once


// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtlog
{


class IMessageIdFilter: virtual public istd::IPolymorphic
{
public:
	enum FilterMode
	{
		FM_INCLUDE = 0,
		FM_EXCLUDE
	};

	virtual QSet<int> GetMessageFilterIds() const = 0;
	virtual FilterMode GetMessageFilterMode() const = 0;
	virtual void SetMessageFilterIds(const QSet<int>& ids) = 0;
	virtual void SetMessageFilterMode(FilterMode mode) = 0;
};


} // namespace imtlog


