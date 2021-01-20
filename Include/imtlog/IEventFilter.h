#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/IInformationProvider.h>

// ImtCore includes
#include <imtlog/ITimeRangeProvider.h>
#include <imtlog/IMessageFilterParams.h>


namespace imtlog
{


class IEventFilter: virtual public istd::IPolymorphic
{
public:

	virtual bool IsMessageAccepted(
				const istd::IInformationProvider& message, 
				const imtlog::CTimeRange* timeRangePtr,
				const IMessageFilterParams* idFilterParamsPtr) const = 0;
};


} // namespace imtlog


