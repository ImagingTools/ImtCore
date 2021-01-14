#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/IInformationProvider.h>

// ImtCore includes
#include <imtlog/ITimeRangeProvider.h>


namespace imtlog
{


class IEventFilter: virtual public ITimeRangeProvider
{
public:
	enum FilterMode
	{
		FM_INCLUDE = 0,
		FM_EXCLUDE
	};

	virtual bool IsMessageAccepted(const istd::IInformationProvider* messagePtr = nullptr) const = 0;

	// Message ids list for search corresponding group
	// Filtering messages by id is implemented by the IsMessageAccepted method
	virtual QList<int> GetGroupMessageIds() const = 0;
};


} // namespace imtlog


